#include "VoxelManagerObject.hpp"

#include <GameObjects/Entities/PlayerObject.hpp>

VoxelManagerObject::VoxelManagerObject() :
	m_voxelManager(),
	m_terrainShader("TerrainShader.glsl"),
	m_terrainAtlas("TerrainTexture.bmp") {}

void VoxelManagerObject::onInitialization() {
	m_terrainAtlas.setActiveSlot(atlasTextureSlot);
}

void VoxelManagerObject::onProcess() {
	// Upload.
	m_voxelManager.getChunkManager()->upload();
}

void VoxelManagerObject::onDraw() {
	// Get camera position.
	PlayerObject* const player = Window.getGameObjectManager()->getFirstGameObjectOfType<PlayerObject>();
	if (player == nullptr) return;
	const Vector3& activePosition = player->getPosition();

	// Get player location.
	const VoxelLocation playerLocation(
		Math::floor(activePosition.x()),
		0,
		Math::floor(activePosition.z())
	);

	// Process + Draw.
	m_voxelManager.getChunkManager()->process(playerLocation, m_terrainShader);

	// Get shader uniforms.
	const ShaderUniform* const u_playerPosition = m_terrainShader.getUniform("u_playerPosition");
	const ShaderUniform* const u_renderDistance = m_terrainShader.getUniform("u_renderDistance");
	const ShaderUniform* const u_voxelTexture = m_terrainShader.getUniform("u_voxelTexture");
	const ShaderUniform* const u_voxelTextureSize = m_terrainShader.getUniform("u_voxelTextureSize");

	// Update shader uniforms..
	glUseProgram(m_terrainShader.getProgram());
	if (u_playerPosition != nullptr) glUniform3f(u_playerPosition->m_location, activePosition.x(), activePosition.y(), activePosition.z());
	if (u_renderDistance != nullptr) glUniform1f(u_renderDistance->m_location, Math::max<float>((voxelChunkViewDistance - 1) * Math::min(voxelChunkSizeX, voxelChunkSizeZ), 1.0f));
	if (u_voxelTexture != nullptr) glUniform1i(u_voxelTexture->m_location, atlasTextureSlot);
	if (u_voxelTextureSize != nullptr) glUniform2f(u_voxelTextureSize->m_location, 4.0f, 4.0f);
	glUseProgram(0);
}
