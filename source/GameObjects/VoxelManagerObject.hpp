#pragma once
#include <Engine/NoisyTerrain.hpp>

#include <Engine/Voxel/VoxelManager.hpp>

class VoxelManagerObject : public GameObject {
private:
	static constexpr GLint atlasTextureSlot = 1;

private:
	VoxelManager m_voxelManager;
	Shader m_terrainShader;
	Texture m_terrainAtlas;

public:
	VoxelManagerObject();

public:
	J_GETTER_DIRECT_MUT(getVoxelManager, &m_voxelManager, VoxelManager* const);
	J_GETTER_DIRECT_MUT(getChunkManager, m_voxelManager.getChunkManager(), ChunkManager* const);

public:
	void onInitialization() override;
	void onProcess() override;
	void onDraw() override;
};