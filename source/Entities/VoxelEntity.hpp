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
		// Upload.
		m_voxelManager.getChunkManager()->upload();
	}

	void onDraw() {
		// Get player location.
		const VoxelLocation playerLocation(
			floorf(CameraEntity::activeCameraPosition.x()),
			0,
			floorf(CameraEntity::activeCameraPosition.z())
		);

		// Process + Draw.
		m_voxelManager.getChunkManager()->process(playerLocation, m_terrainShader);

		// Update view projection.
		const ShaderUniform* const u_viewProjection = m_terrainShader.getUniform("u_viewProjection");
		if (u_viewProjection != nullptr) {
			glUseProgram(m_terrainShader.getProgram());
			glUniformMatrix4fv(u_viewProjection->m_location, 1, GL_FALSE, Draw.getViewProjection().getData());
			glUseProgram(0);
		}
	}
};