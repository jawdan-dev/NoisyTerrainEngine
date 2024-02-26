#pragma once
#include <NoisyTerrain/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>
#include <Voxel/VoxelManager.hpp>

class VoxelEntity : public Entity {
private:
	VoxelManager m_voxelManager;
	Shader m_terrainShader;

	GLuint m_voxelTexture;

public:
	VoxelEntity() : m_voxelManager(), m_terrainShader("TerrainShader.glsl") {}

public:
	void onReady() {
		const Vector3 g1(0.0f, 1.0f, 0.5f), g2(0.0f, 0.75f, 1.0f);
		const Vector3 d1(0.5f, 0.25f, 0.1f), d2(0.75f, 0.5f, 0.0f);
		const Vector3 b1(0.1f, 0.1f, 0.1f), b2(0.05f, 0.05f, 0.05f);
		const Vector3 s1(0.5f, 0.5f, 0.5f), s2(0.3f, 0.4f, 0.5f);
		const Vector3 m1(1.0f, 0.0f, 1.0f), m2(0.0f, 0.0f, 0.0f);

		const Vector3 textureData[3 * 3 * 4] {
			m1, m2, g1, g2, g1, g2,
			m2, m1, g2, g1, d2, d1,
			d1, d2, s1, s2, b1, b2,
			d2, d1, s2, s1, b2, b1,
			m1, m2, m1, m2, m1, m2,
			m2, m1, m2, m1, m2, m1,
		};
		glGenTextures(1, &m_voxelTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_voxelTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 6, 6, 0, GL_RGB, GL_FLOAT, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

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
		const ShaderUniform* const u_voxelTexture = m_terrainShader.getUniform("u_voxelTexture");
		const ShaderUniform* const u_voxelTextureSize = m_terrainShader.getUniform("u_voxelTextureSize");

		// Update shader uniforms..
		glUseProgram(m_terrainShader.getProgram());
		if (u_viewProjection != nullptr) glUniformMatrix4fv(u_viewProjection->m_location, 1, GL_FALSE, Draw.getViewProjection().getData());
		if (u_playerPosition != nullptr) glUniform3f(u_playerPosition->m_location, CameraEntity::activeCameraPosition.x(), CameraEntity::activeCameraPosition.y(), CameraEntity::activeCameraPosition.z());
		if (u_renderDistance != nullptr) glUniform1f(u_renderDistance->m_location, __max((voxelChunkViewDistance - 1) * __min(voxelChunkSizeX, voxelChunkSizeZ), 1.0f));
		if (u_voxelTexture != nullptr) glUniform1ui(u_voxelTexture->m_location, 0);
		if (u_voxelTextureSize != nullptr) glUniform2f(u_voxelTextureSize->m_location, 3.0f, 3.0f);
		glUseProgram(0);
	}
};