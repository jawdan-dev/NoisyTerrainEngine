#include "ChunkManager.hpp"

#include <NoisyTerrain/ThreadPool/ThreadPool.hpp>

ChunkManager::ChunkManager(VoxelManager* const voxelManager) :
	m_voxelManager(voxelManager),
	m_chunkMutex(), m_chunks(),
	m_initializationMutex(), m_placementMutex(), m_rebuildMutex(), m_drawMutex() {}
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

void ChunkManager::process(const ChunkLocation& processCenter) {
	// Try to grab ThreadPool.
	if (ThreadPool.tryLock()) {
		// Check active jobs.
		m_activeJobsMutex.lock();
		if (!ThreadPool.getJobsActiveMut(m_activeJobsList)) {
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

			// Thread safety.
			lockChunks();

			// Initialize chunks.
			size_t chunksRemaining = voxelChunkBatchSize;
			for (size_t i = 0; i < chunkLocations.size() && chunksRemaining > 0; i++) {
				const ChunkLocation location = processCenter + chunkLocations[i];
				Chunk* const chunk = getChunk(location);

				if (chunk == nullptr || chunk->hasLoaded()) continue;

				chunk->queueInitialization();
				chunksRemaining--;
			}

			// Job staging helper.
			List<ThreadJobID> tempJobs;
#			define StartStage(targetQueue, targetLock, jobFunction, nextJob) \
				/* Enqueue creation job. */ \
				m_activeJobsList.push_back(ThreadPool.enqueueJob([this](){ \
					/* Thread safety locks. */ \
					m_activeJobsMutex.lock(); \
					targetLock.lock(); \
					ThreadPool.lock(); \
					for (auto it = targetQueue.begin(); it != targetQueue.end(); it++) { \
						/* Get chunk. */ \
						Chunk* const chunk = getChunk(*it); \
						if (chunk == nullptr) continue; \
						/* Start job. */ \
						m_activeJobsList.push_back(ThreadPool.enqueueJob([this, chunk]() { \
							jobFunction; \
						})); \
					} \
					/* Update details */ \
					targetQueue.clear(); \
					targetLock.unlock(); \
					/* Queue up next job. */ \
					nextJob \
					/* Free job mutex. */ \
					m_activeJobsMutex.unlock(); \
					ThreadPool.unlock(); \
				}, m_activeJobsList));

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
							m_pendingModelsList.push_back(chunk->getModel()); \
							m_pendingModelsMutex.unlock()
						),
					)));

			// TODO: Queue (un)draw.

			// Safely threaded.
			unlockChunks();
		}

		// Free locks.
		m_activeJobsMutex.unlock();
		ThreadPool.unlock();
	}

	// Upload models.
	m_pendingModelsMutex.lock();
	for (auto it = m_pendingModelsList.begin(); it != m_pendingModelsList.end();) {
		// Get model.
		Model& model = **it;
		if (model.tryLock()) {
			// Upload.
			if (model.upload()) it = m_pendingModelsList.erase(it);
			else it++;

			// Unlock.
			model.unlock();
		} else {
			it++;
		}
	}
	m_pendingModelsMutex.unlock();
}
void ChunkManager::draw(Shader& shader) {
	lockDraw();
	if (m_drawQueue.size() <= 0) {
		unlockDraw();
		return;
	}

	// Draw all in queue.
	InstanceData instanceData(&shader);
	for (auto it = m_drawQueue.begin(); it != m_drawQueue.end(); it++) {
		Chunk* const chunk = getChunk(*it);
		chunk->forceDraw(instanceData);
	}

	// Update details.
	m_drawQueue.clear();
	unlockDraw();
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