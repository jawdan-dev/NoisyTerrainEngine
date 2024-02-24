#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Chunk/Chunk.hpp>
#include <NoisyTerrain/ThreadPool/ThreadJob.hpp>

class VoxelManager;

class ChunkManager {
private:
	VoxelManager* const m_voxelManager;

private:
	Mutex m_chunkMutex;
	Map<ChunkLocation, Chunk> m_chunks;

private:
	// Job stuff.
	enum class ManagerState : uint8_t {
		None = 0, _Reserved,

		WaitForCooldown,
		WaitForJobs,

		SelectBatch,
		Initialization,
		Placement,
		Rebuild,
		Visibility,
		Undraw,
		_Draw
	};
	ManagerState m_activeState, m_waitState;
	static constexpr float c_activeJobCooldownMax = 0.2f;
	float m_activeJobCooldown;
	Mutex m_activeJobMutex;
	List<ThreadJobID> m_activeJobList;

private:
	// Upload stuff.
	Mutex m_pendingModelsMutex;
	Set<Model*> m_pendingModelsList;

private:
	// State stuff.
	Mutex m_initializationMutex, m_placementMutex, m_rebuildMutex, m_visibilityMutex, m_drawMutex, m_undrawMutex;
	Set<ChunkLocation> m_initializationQueue, m_placementQueue, m_visibilityQueue, m_rebuildQueue, m_drawQueue, m_undrawQueue;
	ChunkLocation m_visibilityCenter;

public:
	ChunkManager(VoxelManager* const voxelManager);
	ChunkManager(const ChunkManager& other) = delete;
	~ChunkManager();

public:
	J_GETTER_DIRECT_MUT(getVoxelManager, m_voxelManager, VoxelManager* const);
	const VoxelID getVoxel(const VoxelLocation& location);
	Chunk* const getChunk(const ChunkLocation& location);

public:
	void trySetVoxel(const VoxelLocation& location, const VoxelID voxelID);
	void setVoxel(const VoxelLocation& location, const VoxelID voxelID);

public:
	void process(const ChunkLocation& visibilityCenter, Shader& shader);
	void upload();

public:
	void lockChunks();
	void unlockChunks();

public:
	void queueInitialization(const ChunkLocation& location);
	void lockInitialization();
	void unlockInitialization();

public:
	void queuePlacement(const ChunkLocation& location);
	void lockPlacement();
	void unlockPlacement();

public:
	void queueRebuild(const ChunkLocation& location);
	void lockRebuild();
	void unlockRebuild();

public:
	void queueDraw(const ChunkLocation& location);
	void lockDraw();
	void unlockDraw();

public:
	void queueUndraw(const ChunkLocation& location);
	void lockUndraw();
	void unlockUndraw();
};