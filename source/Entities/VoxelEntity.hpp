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

		// Get shader uniforms.
		const ShaderUniform* const u_viewProjection = m_terrainShader.getUniform("u_viewProjection");
		const ShaderUniform* const u_playerPosition = m_terrainShader.getUniform("u_playerPosition");
		const ShaderUniform* const u_renderDistance = m_terrainShader.getUniform("u_renderDistance");

		// Update shader uniforms..
		glUseProgram(m_terrainShader.getProgram());
		if (u_viewProjection != nullptr) glUniformMatrix4fv(u_viewProjection->m_location, 1, GL_FALSE, Draw.getViewProjection().getData());
		if (u_playerPosition != nullptr) glUniform3f(u_playerPosition->m_location, CameraEntity::activeCameraPosition.x(), CameraEntity::activeCameraPosition.y(), CameraEntity::activeCameraPosition.z());
		if (u_renderDistance != nullptr) glUniform1f(u_renderDistance->m_location, __max((voxelChunkViewDistance - 1) * __min(voxelChunkSizeX, voxelChunkSizeZ), 1.0f));
		glUseProgram(0);
	}
};