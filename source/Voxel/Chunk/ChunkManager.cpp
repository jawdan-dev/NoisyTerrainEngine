#include "ChunkManager.hpp"

#include <NoisyTerrain/Time/Time.hpp>
#include <NoisyTerrain/ThreadPool/ThreadPool.hpp>

ChunkManager::ChunkManager(VoxelManager* const voxelManager) :
	m_voxelManager(voxelManager),
	m_chunkMutex(), m_chunks(),
	// Job stuff.
	m_activeState(ManagerState::WaitForCooldown), m_waitState(ManagerState::None),
	m_activeJobCooldown(0),
	m_activeJobMutex(), m_activeJobList(),
	// Upload stuff.
	m_pendingModelsMutex(), m_pendingModelsList(),
	// State stuff.
	m_initializationMutex(), m_placementMutex(), m_rebuildMutex(), m_visibilityMutex(), m_drawMutex(), m_undrawMutex(),
	m_initializationQueue(), m_placementQueue(), m_rebuildQueue(), m_visibilityQueue(), m_drawQueue(), m_undrawQueue(),
	m_visibilityCenter() {}
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

void ChunkManager::process(const ChunkLocation& visibilityCenter, Shader& shader) {
	// Set visibility center.
	if (m_visibilityMutex.try_lock()) {
		// Set center.
		m_visibilityCenter = visibilityCenter;
		m_visibilityMutex.unlock();
	}

	// Check job cooldown.
	if (m_activeJobCooldown > 0)
		m_activeJobCooldown -= Time.delta();

	// Thread safely.
	bool locks[3];
	if (!(locks[0] = m_chunkMutex.try_lock()) ||
		!(locks[1] = m_activeJobMutex.try_lock()) ||
		!(locks[2] = ThreadPool.tryLock())) {
		// Failed, unlock.
		if (locks[2]) ThreadPool.unlock();
		if (locks[1]) m_activeJobMutex.unlock();
		if (locks[0]) m_chunkMutex.unlock();
		return;
	}

#pragma region Macros
#	define StageJobs(targetQueue, targetLock, ...) \
		/* Enqueue creation job. */ \
		m_activeJobList.push_back(ThreadPool.enqueueJob([this](){ \
			/* Thread safety locks. */ \
			targetLock.lock(); \
			lockChunks(); \
			m_activeJobMutex.lock(); \
			ThreadPool.lock(); \
			for (auto it = targetQueue.begin(); it != targetQueue.end(); it++) { \
				/* Get chunk. */ \
				Chunk* const chunk = getChunk(*it); \
				if (chunk == nullptr) continue; \
				/* Start job. */ \
				m_activeJobList.push_back(ThreadPool.enqueueJob([this, chunk]() { \
					chunk->lockDetails(); \
					__VA_ARGS__; \
					chunk->unlockDetails(); \
				})); \
			} \
			/* Free job locks. */ \
			ThreadPool.unlock(); \
			m_activeJobMutex.unlock(); \
			unlockChunks(); \
			/* Update details */ \
			targetQueue.clear(); \
			targetLock.unlock(); \
		}, m_activeJobList))
#pragma endregion

	// Perform as many states as possible.
	ManagerState checkState = ManagerState::_Reserved;
	while (checkState != m_activeState) {
		// Update check state.
		checkState = m_activeState;

		// Handle state.
		switch (m_activeState) {
			case ManagerState::WaitForCooldown: {
				if (m_activeJobCooldown > 0) break;

				// Change state.
				m_activeState = ManagerState::SelectBatch;
				m_activeJobCooldown = c_activeJobCooldownMax;
			} break;
			case ManagerState::WaitForJobs: {
				if (ThreadPool.getJobsActiveMut(m_activeJobList)) break;

				// Update state.
				m_activeState = m_waitState;
				m_waitState = ManagerState::None;
			} break;

			case ManagerState::SelectBatch: {
				// Get chunk location check order.
				static List<ChunkLocation> chunkLocations;
				const VoxelInt chunkViewDistanceChunkCount = ((voxelChunkViewDistance * 2) + 1) * ((voxelChunkViewDistance * 2) + 1);
				if (chunkLocations.size() != chunkViewDistanceChunkCount) {
					// Create list of chunks to load.
					chunkLocations.clear();
					for (VoxelInt x = -voxelChunkViewDistance; x <= voxelChunkViewDistance; x++) {
						for (VoxelInt z = -voxelChunkViewDistance; z <= voxelChunkViewDistance; z++) {
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
					if (chunkLocations.size() != chunkViewDistanceChunkCount) {
						J_WARNING("ChunkManager.cpp: Failed to generate render generation order, instead got %zu of expected %zu chunks.\n", chunkLocations.size(), chunkViewDistanceChunkCount);
					} else {
						J_LOG("ChunkManager.cpp: Render generation order created for %zu chunks.\n", chunkLocations.size());
					}
				}

				// Select batch.
				size_t chunksRemaining = voxelChunkBatchSize;
				for (size_t i = 0; i < chunkLocations.size() && chunksRemaining > 0; i++) {
					// Get chunk.
					const ChunkLocation location = m_visibilityCenter + chunkLocations[i];
					Chunk* const chunk = getChunk(location);
					if (chunk == nullptr) continue;

					chunk->lockDetails();
					if (!chunk->isVisible()) {
						// Queue draw + initialization.
						chunk->queueInitialization();
						chunk->queueRebuild();
						chunk->queueDraw();

						// Update details.
						chunksRemaining--;
					}
					chunk->unlockDetails();
				}

				// Next state.
				m_waitState = ManagerState::Initialization;
			} break;

			case ManagerState::Initialization: {
				// Initialize.
				StageJobs(m_initializationQueue, m_initializationMutex,
					chunk->lockPlacement(); \
					chunk->forceInitialization(); \
					chunk->unlockPlacement();
				);
				// Next state.
				m_waitState = ManagerState::Placement;
			} break;
			case ManagerState::Placement: {
				// Initialize.
				StageJobs(m_placementQueue, m_placementMutex,
					chunk->lockPlacement(); \
					chunk->forcePlacement(); \
					chunk->unlockPlacement();
				);
				// Next state.
				m_waitState = ManagerState::Rebuild;
			} break;
			case ManagerState::Rebuild: {
				// Rebuild.
				StageJobs(m_rebuildQueue, m_rebuildMutex,
					chunk->forceRebuild(); \
					m_pendingModelsMutex.lock(); \
					m_pendingModelsList.emplace(chunk->getModel()); \
					m_pendingModelsMutex.unlock(); \
				);
				// Next state.
				m_waitState = ManagerState::Visibility;
			} break;
			case ManagerState::Visibility: {
				// Check visibility.
				m_activeJobList.push_back(ThreadPool.enqueueJob([this]() {
					/* Thread safety locks. */
					m_visibilityMutex.lock();
					lockChunks();
					m_activeJobMutex.lock();
					ThreadPool.lock();
					for (auto it = m_visibilityQueue.begin(); it != m_visibilityQueue.end(); it++) {
						/* Get chunk. */
						Chunk* const chunk = getChunk(*it);
						if (chunk == nullptr) continue;

						m_activeJobList.push_back(ThreadPool.enqueueJob([this, chunk]() {
							// Lock.
							chunk->lockDetails();

							// Get distance.
							const ChunkLocation diff = chunk->getChunkLocation() - m_visibilityCenter;
							const VoxelInt dist = __max(abs(diff.x()), abs(diff.z()));

							// Handle if shown or hidden.
							if (dist <= voxelChunkViewDistance) {
								if (!chunk->isVisible()) chunk->queueDraw();
							} else {
								if (chunk->isVisible()) chunk->queueUndraw();
							}

							// Unlock.
							chunk->unlockDetails();
							}));
					}
					// Unlock visibility mutex.
					ThreadPool.enqueueJob([this]() {
						m_visibilityMutex.unlock();
					}, m_activeJobList);

					/* Free job locks. */
					ThreadPool.unlock();
					m_activeJobMutex.unlock();
					unlockChunks();
					}, m_activeJobList)
				);
				// Next state.
				m_waitState = ManagerState::Undraw;
			} break;
			case ManagerState::Undraw: {
				if (!m_visibilityMutex.try_lock()) break;
				if (!m_undrawMutex.try_lock()) {
					m_visibilityMutex.unlock();
					break;
				}

				// Undraw.
				for (auto it = m_undrawQueue.begin(); it != m_undrawQueue.end(); it++) {
					// Get chunk.
					Chunk* const chunk = getChunk(*it);
					if (chunk == nullptr) continue;

					// Undraw.
					chunk->lockDetails();
					chunk->forceUndraw();
					chunk->unlockDetails();

					// Remove from visibility queue.
					auto visibilityIt = m_visibilityQueue.find(*it);
					if (visibilityIt != m_visibilityQueue.end())
						m_visibilityQueue.erase(visibilityIt);
				}
				// Update details.
				m_undrawQueue.clear();

				// Unlock.
				m_undrawMutex.unlock();
				m_visibilityMutex.unlock();

				// Next state.
				m_activeState = ManagerState::_Draw;
			} break;
			case ManagerState::_Draw: {
				if (!m_visibilityMutex.try_lock()) break;
				if (!m_drawMutex.try_lock()) {
					m_visibilityMutex.unlock();
					break;
				}

				// Draw.
				InstanceData instanceData(&shader);
				for (auto it = m_drawQueue.begin(); it != m_drawQueue.end(); it++) {
					// Get chunk.
					Chunk* const chunk = getChunk(*it);
					if (chunk == nullptr) continue;

					// Draw.
					chunk->lockDetails();
					chunk->forceDraw(instanceData);
					chunk->unlockDetails();

					// Add to visibility queue.
					m_visibilityQueue.emplace(*it);
				}
				// Update details.
				m_drawQueue.clear();

				// Unlock.
				m_drawMutex.unlock();
				m_visibilityMutex.unlock();

				// Next state.
				m_waitState = ManagerState::WaitForCooldown;
			} break;
		}

		// Check next state.
		if (m_waitState != ManagerState::None) {
			m_activeState = ManagerState::WaitForJobs;
		}
	}

	// Clean up macros.
#	undef StageJobs

	// Safely threaded.
	ThreadPool.unlock();
	m_activeJobMutex.unlock();
	m_chunkMutex.unlock();
}
void ChunkManager::upload() {
	if (!m_pendingModelsMutex.try_lock()) return;
	if (m_pendingModelsList.size() <= 0) {
		m_pendingModelsMutex.unlock();
		return;
	}

	// Get dynamic uplaod amount.
	const size_t uploadAmount = __max(chunkUploadSpeed / m_pendingModelsList.size(), 20);

	// Upload.
	for (auto it = m_pendingModelsList.begin(); it != m_pendingModelsList.end();) {
		// Get model.
		Model& model = **it;
		if (model.tryLock()) {
			// Upload.
			if (!model.upload(uploadAmount)) it = m_pendingModelsList.erase(it);
			else it++;

			// Unlock.
			model.unlock();
		} else {
			it++;
		}
	}
	m_pendingModelsMutex.unlock();
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