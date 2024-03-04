#pragma once
#include <Engine/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>
#include <Engine/Voxel/VoxelManager.hpp>

class VoxelEntity : public Entity {
private:
	static constexpr GLint atlasTextureSlot = 1;

private:
	VoxelManager m_voxelManager;
	Shader m_terrainShader;
	Texture m_terrainAtlas;

public:
	VoxelEntity() :
		m_voxelManager(),
		m_terrainShader("TerrainShader.glsl"),
		m_terrainAtlas("TerrainTexture.bmp") {}

public:
	J_GETTER_DIRECT_MUT(getVoxelManager, &m_voxelManager, VoxelManager* const);
	J_GETTER_DIRECT_MUT(getChunkManager, m_voxelManager.getChunkManager(), ChunkManager* const);

public:
	void onReady() {
		m_terrainAtlas.setActiveSlot(atlasTextureSlot);
	}

	void onProcess() {
		// Upload.
		m_voxelManager.getChunkManager()->upload();
	}

	void onDraw() {
		// Get player location.
		const VoxelLocation playerLocation(
			Math::floor(CameraEntity::activeCameraPosition.x()),
			0,
			Math::floor(CameraEntity::activeCameraPosition.z())
		);

		// Process + Draw.
		m_voxelManager.getChunkManager()->process(playerLocation, m_terrainShader);

		// Get shader uniforms.
		const ShaderUniform* const u_viewProjection = m_terrainShader.getUniform("u_viewProjection");
		const ShaderUniform* const u_playerPosition = m_terrainShader.getUniform("u_playerPosition");
		const ShaderUniform* const u_renderDistance = m_terrainShader.getUniform("u_renderDistance");
		const ShaderUniform* const u_voxelTexture = m_terrainShader.getUniform("u_voxelTexture");
		const ShaderUniform* const u_voxelTextureSize = m_terrainShader.getUniform("u_voxelTextureSize");

		// Update shader uniforms..
		glUseProgram(m_terrainShader.getProgram());
		if (u_viewProjection != nullptr) glUniformMatrix4fv(u_viewProjection->m_location, 1, GL_FALSE, Draw.getViewProjection().getData());
		if (u_playerPosition != nullptr) glUniform3f(u_playerPosition->m_location, CameraEntity::activeCameraPosition.x(), CameraEntity::activeCameraPosition.y(), CameraEntity::activeCameraPosition.z());
		if (u_renderDistance != nullptr) glUniform1f(u_renderDistance->m_location, Math::max<float>((voxelChunkViewDistance - 1) * Math::min(voxelChunkSizeX, voxelChunkSizeZ), 1.0f));
		if (u_voxelTexture != nullptr) glUniform1i(u_voxelTexture->m_location, atlasTextureSlot);
		if (u_voxelTextureSize != nullptr) glUniform2f(u_voxelTextureSize->m_location, 4.0f, 4.0f);
		glUseProgram(0);
	}
};