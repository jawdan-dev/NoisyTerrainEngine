#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Voxel/Chunk/ChunkManager.hpp>
#include <Engine/Voxel/Layer/LayerManager.hpp>

class VoxelManager {
private:
	LayerManager m_layerManager;
	ChunkManager m_chunkManager;

public:
	VoxelManager();
	VoxelManager(const VoxelManager& other) = delete;
	~VoxelManager();

public:
	J_GETTER_DIRECT_MUT(getLayerManager, &m_layerManager, LayerManager* const);
	J_GETTER_DIRECT_MUT(getChunkManager, &m_chunkManager, ChunkManager* const);
};