#pragma once
#include <NoisyTerrain/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>
#include <Voxel/VoxelManager.hpp>

class VoxelEntity : public Entity {
private:
	VoxelManager m_voxelManager;
	Shader m_terrainShader;

public:
	VoxelEntity() : m_voxelManager(), m_terrainShader("TerrainShader.glsl") {}

public:
	void onProcess() {
		// Get player location.
		const VoxelLocation playerLocation(
			floorf(CameraEntity::activeCameraPosition.x()),
			0,
			floorf(CameraEntity::activeCameraPosition.z())
		);

		// Process.
		m_voxelManager.getChunkManager()->process(playerLocation);
	}

	void onDraw() {
		// Draw.
		m_voxelManager.getChunkManager()->draw(m_terrainShader);
	}
};