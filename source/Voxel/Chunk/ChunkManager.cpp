#include "ChunkManager.hpp"

#include <NoisyTerrain/Time/Time.hpp>
#include <NoisyTerrain/ThreadPool/ThreadPool.hpp>

ChunkManager::ChunkManager(VoxelManager* const voxelManager) :
	m_voxelManager(voxelManager),
	m_chunkMutex(), m_chunks(),
	m_activeJobCooldown(0), m_activeJobMutex(), m_activeJobList(),
	m_visibilityMutex(), m_visibilityQueue(), m_visibilityCenter(),
	m_pendingModelsMutex(), m_pendingModelsList(),
	m_initializationMutex(), m_placementMutex(), m_rebuildMutex(), m_drawMutex(), m_undrawMutex(),
	m_initializationQueue(), m_placementQueue(), m_rebuildQueue(), m_drawQueue(), m_undrawQueue() {}
ChunkManager::~ChunkManager() {}

const VoxelID ChunkManager::getVoxel(const VoxelLocation& location) {
	// Get chunk.
	Chunk* const chunk = getChunk(location);
	if (chunk == nullptr) return VoxelID::None;

	// Get voxel.
	chunk->lockPlacement();
	const VoxelID voxelID = chunk->getVoxel(location.getRelativeLocation());
	chunk->unlockPlacement();

	// Return voxel.
	return voxelID;
}
Chunk* const ChunkManager::getChunk(const ChunkLocation& location) {
	// Get chunk.
	auto it = m_chunks.find(location);
	if (it == m_chunks.end()) {
		// Check world bounds.
		if (location.x() < -worldMaxHorizontalBound || location.x() >= worldMaxHorizontalBound ||
			location.z() < -worldMaxHorizontalBound || location.z() >= worldMaxHorizontalBound)
			return nullptr;

		// Create chunk.
		it = m_chunks.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(location),
			std::forward_as_tuple(this, location)
		).first;
	}

	// Return chunk.
	return &it->second;
}

void ChunkManager::trySetVoxel(const VoxelLocation& location, const VoxelID voxelID) {
	// Get chunk.
	Chunk* const chunk = getChunk(location);
	if (chunk == nullptr) return;

	// Set voxel.
	chunk->lockPlacement();
	chunk->queueTrySetVoxel(location.getRelativeLocation(), voxelID);
	chunk->unlockPlacement();
}
void ChunkManager::setVoxel(const VoxelLocation& location, const VoxelID voxelID) {
	// Get chunk.
	Chunk* const chunk = getChunk(location);
	if (chunk == nullptr) return;

	// Set voxel.
	chunk->lockPlacement();
	chunk->queueSetVoxel(location.getRelativeLocation(), voxelID);
	chunk->unlockPlacement();
}

void ChunkManager::process(const ChunkLocation& visibilityCenter) {
	// Set visibility center.
	if (m_visibilityMutex.try_lock()) {
		// Set center.
		m_visibilityCenter = visibilityCenter;
		m_visibilityMutex.unlock();
	}

	// Check job cooldown.
	m_activeJobCooldown -= Time.delta();
	if (m_activeJobCooldown > 0) return;

	// Grab locks.
	bool locks[3];
	if (!(locks[0] = m_chunkMutex.try_lock()) ||
		!(locks[1] = m_activeJobMutex.try_lock()) ||
		!(locks[2] = ThreadPool.tryLock()) ||
		ThreadPool.getJobsActiveMut(m_activeJobList)) {
		// Failed, unlock.
		if (locks[2]) ThreadPool.unlock();
		if (locks[1]) m_activeJobMutex.unlock();
		if (locks[0]) m_chunkMutex.unlock();
		return;
	}

	// Get chunk location check order.
	static List<ChunkLocation> chunkLocations;
	const size_t chunkViewDistanceChunkCount = (voxelChunkViewDistance * 2) * (voxelChunkViewDistance * 2);
	if (chunkLocations.size() != chunkViewDistanceChunkCount) {
		// Create list of chunks to load.
		chunkLocations.clear();
		for (VoxelInt x = -voxelChunkViewDistance; x < voxelChunkViewDistance; x++) {
			for (VoxelInt z = -voxelChunkViewDistance; z < voxelChunkViewDistance; z++) {
				chunkLocations.push_back(ChunkLocation(x, z));
			}
		}

		// Sort into generation order.
		struct LocationComparator {
			inline bool operator() (const ChunkLocation& a, const ChunkLocation& b) {
				return __max(abs(a.x()), abs(a.z())) < __max(abs(b.x()), abs(b.z()));
			}
		};
		std::sort(chunkLocations.begin(), chunkLocations.end(), LocationComparator());

		// Log.
		J_LOG("ChunkManager.cpp: Render generation order created for %zu chunks.\n", chunkLocations.size());
	}

	// Initialize chunks.
	size_t chunksRemaining = voxelChunkBatchSize;
	for (size_t i = 0; i < chunkLocations.size() && chunksRemaining > 0; i++) {
		const ChunkLocation location = m_visibilityCenter + chunkLocations[i];
		Chunk* const chunk = getChunk(location);

		if (chunk == nullptr || chunk->hasDrawn()) continue;

		chunk->queueInitialization();
		chunk->queueDraw();
		chunksRemaining--;
	}

	// Job staging helper.
	List<ThreadJobID> tempJobs;
#	define StartStage(targetQueue, targetLock, jobFunction, nextJob) \
		/* Enqueue creation job. */ \
		m_activeJobList.push_back(ThreadPool.enqueueJob([this](){ \
			/* Thread safety locks. */ \
			targetLock.lock(); \
			m_activeJobMutex.lock(); \
			ThreadPool.lock(); \
			lockChunks(); \
			for (auto it = targetQueue.begin(); it != targetQueue.end(); it++) { \
				/* Get chunk. */ \
				Chunk* const chunk = getChunk(*it); \
				if (chunk == nullptr) continue; \
				/* Start job. */ \
				m_activeJobList.push_back(ThreadPool.enqueueJob([this, chunk]() { \
					jobFunction; \
				})); \
			} \
			/* Update details */ \
			targetQueue.clear(); \
			targetLock.unlock(); \
			/* Queue up next job. */ \
			nextJob \
			/* Free job mutexes. */ \
			unlockChunks(); \
			ThreadPool.unlock(); \
			m_activeJobMutex.unlock(); \
		}, m_activeJobList));

			// Stage jobs:
	StartStage(
		// Initialize.
		m_initializationQueue, m_initializationMutex,
		ARGS(chunk->forceInitialization()),
		StartStage(
			// Place.
			m_placementQueue, m_placementMutex,
			ARGS(chunk->lockPlacement(); chunk->forcePlacement(); chunk->unlockPlacement()),
			StartStage(
				// Rebuild.
				m_rebuildQueue, m_rebuildMutex,
				ARGS(
					chunk->forceRebuild(); \
					m_pendingModelsMutex.lock(); \
					m_pendingModelsList.emplace(chunk->getModel()); \
					m_pendingModelsMutex.unlock(); \
					chunk->queueDraw()
				),
				m_activeJobList.push_back(ThreadPool.enqueueJob([this]() {
					m_visibilityMutex.lock();
					m_activeJobMutex.lock();
					ThreadPool.lock();
					lockChunks();
					for (auto it = m_visibilityQueue.begin(); it != m_visibilityQueue.end(); it++) {
						Chunk* const chunk = getChunk(*it);
						if (chunk == nullptr) continue;

						m_activeJobList.push_back(ThreadPool.enqueueJob([this, chunk]() {
							const ChunkLocation diff = m_visibilityCenter - chunk->getChunkLocation();
							const VoxelInt dist = __max(abs(diff.x()), abs(diff.z()));
							if (dist <= voxelChunkViewDistance) {
								chunk->queueDraw();
							} else {
								chunk->queueUndraw();
							}
							}));
					}
					unlockChunks();
					ThreadPool.unlock();
					m_activeJobMutex.unlock();
					m_visibilityMutex.unlock();
					}, m_activeJobList));
	)));

	// Safely threaded.
	unlockChunks();
	m_activeJobMutex.unlock();
	ThreadPool.unlock();

	// Update cooldown.
	m_activeJobCooldown = activeJobCooldownMax;
}
void ChunkManager::upload() {
	// Upload models.
	if (m_pendingModelsMutex.try_lock()) {
		for (auto it = m_pendingModelsList.begin(); it != m_pendingModelsList.end();) {
			// Get model.
			Model& model = **it;
			if (model.tryLock()) {
				// Upload.
				if (!model.upload()) it = m_pendingModelsList.erase(it);
				else it++;

				// Unlock.
				model.unlock();
			} else {
				it++;
			}
		}
		m_pendingModelsMutex.unlock();
	}
}
void ChunkManager::draw(Shader& shader) {
	if (m_chunkMutex.try_lock()) {
		// Undraw.
		if (m_undrawMutex.try_lock()) {
			m_visibilityMutex.lock();
			// Undraw all in queue.
			for (auto it = m_undrawQueue.begin(); it != m_undrawQueue.end(); it++) {
				// Get chunk.
				Chunk* const chunk = getChunk(*it);
				if (chunk == nullptr) continue;

				// Undraw.
				chunk->forceUndraw();

				// Remove from visibility queue.
				auto visibilityIt = m_visibilityQueue.find(*it);
				if (visibilityIt != m_visibilityQueue.end())
					m_visibilityQueue.erase(visibilityIt);
			}
			// Update details.
			m_undrawQueue.clear();

			// Unlock.
			m_visibilityMutex.unlock();
			m_undrawMutex.unlock();
		}

		// Draw.
		if (m_drawMutex.try_lock()) {
			m_visibilityMutex.lock();
			// Draw all in queue.
			InstanceData instanceData(&shader);
			for (auto it = m_drawQueue.begin(); it != m_drawQueue.end(); it++) {
				// Get chunk.
				Chunk* const chunk = getChunk(*it);
				if (chunk == nullptr) continue;

				// Draw.
				chunk->forceDraw(instanceData);

				// Add to visibility queue.
				m_visibilityQueue.emplace(*it);
			}
			// Update details.
			m_drawQueue.clear();

			// Unlock.
			m_visibilityMutex.unlock();
			m_drawMutex.unlock();
		}
		m_chunkMutex.unlock();
	}
}

void ChunkManager::lockChunks() {
	m_chunkMutex.lock();
}
void ChunkManager::unlockChunks() {
	m_chunkMutex.unlock();
}

void ChunkManager::queueInitialization(const ChunkLocation& location) {
	m_initializationQueue.emplace(location);
}
void ChunkManager::lockInitialization() {
	m_initializationMutex.lock();
}
void ChunkManager::unlockInitialization() {
	m_initializationMutex.unlock();
}

void ChunkManager::queuePlacement(const ChunkLocation& location) {
	m_placementQueue.emplace(location);
}
void ChunkManager::lockPlacement() {
	m_placementMutex.lock();
}
void ChunkManager::unlockPlacement() {
	m_placementMutex.unlock();
}

void ChunkManager::queueRebuild(const ChunkLocation& location) {
	m_rebuildQueue.emplace(location);
}
void ChunkManager::lockRebuild() {
	m_rebuildMutex.lock();
}
void ChunkManager::unlockRebuild() {
	m_rebuildMutex.unlock();
}

void ChunkManager::queueDraw(const ChunkLocation& location) {
	m_drawQueue.emplace(location);
}
void ChunkManager::lockDraw() {
	m_drawMutex.lock();
}
void ChunkManager::unlockDraw() {
	m_drawMutex.unlock();
}

void ChunkManager::queueUndraw(const ChunkLocation& location) {
	m_undrawQueue.emplace(location);
}
void ChunkManager::lockUndraw() {
	m_undrawMutex.lock();
}
void ChunkManager::unlockUndraw() {
	m_undrawMutex.unlock();
}