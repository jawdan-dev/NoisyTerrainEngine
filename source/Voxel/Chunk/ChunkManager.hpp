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
	Mutex m_activeJobsMutex;
	List<ThreadJobID> m_activeJobsList;

private:
	Mutex m_pendingModelsMutex;
	List<Model*> m_pendingModelsList;

private:
	Mutex m_initializationMutex, m_placementMutex, m_rebuildMutex, m_drawMutex;
	Set<ChunkLocation> m_initializationQueue, m_placementQueue, m_rebuildQueue, m_drawQueue;

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
	void process(const ChunkLocation& processCenter);
	void draw(Shader& shader);

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
};