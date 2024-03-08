#include "CameraObject.hpp"

#include <GameObjects/Entities/PlayerObject.hpp>
#include <GameObjects/VoxelManagerObject.hpp>

void CameraObject::onInitialization() {
	// Set initial position.
	const PlayerObject* const playerObject = Window.getGameObjectManager()->getFirstGameObjectOfType<PlayerObject>();
	if (playerObject != nullptr) setPosition(playerObject->getPosition());

	// Set initial rotation.
	setRotation(Vector3(0, 3.14159265358979f, 0));
}

void CameraObject::onProcess() {
	// Camera config.
	static const float s_cameraZoomSpeed = 0.3f;
	static const Vector3 s_cameraOffset = Vector3(0.0f, 1.2f, 0.0f);
	static const Vector2 s_cameraDistanceBounds(1.0f, 10.0f);
	static const float s_cameraFollowStrength = 4.0f;
	static const Vector3 s_cameraFollowVelocityFactor = Vector3(1.0f, 0.3f, 1.0f) * 0.25f;
	static const float s_cameraSensitivity = 0.5f;
	static const float s_cameraVerticalViewAngle = (80.0f / 180.0f) * Math::PI;

	// Get player.
	const PlayerObject* const playerObject = Window.getGameObjectManager()->getFirstGameObjectOfType<PlayerObject>();
	if (playerObject == nullptr) return;

	// Get voxel + chunk manager.
	VoxelManagerObject* const voxelManager = Window.getGameObjectManager()->getFirstGameObjectOfType<VoxelManagerObject>();
	if (voxelManager == nullptr) return;
	ChunkManager* const chunkManager = voxelManager->getChunkManager();

	// Get input.
	int windowCenterWidth = Window.getWidth() / 2, windowCenterHeight = Window.getHeight() / 2;
	const Vector3 cameraInput(
		m_mouseLocked ? (windowCenterWidth - Input.getMouseX()) : 0.0f,
		m_mouseLocked ? (windowCenterHeight - Input.getMouseY()) : 0.0f,
		m_mouseLocked ? -Input.getMouseScrollY() : 0.0f
	);

	// Center mouse.
	if (Input.getKeyDown(GLFW_KEY_TAB) || (m_mouseLocked && !Window.isFocused())) {
		m_mouseLocked = !m_mouseLocked;
		glfwSetInputMode(Window.getWindowContext(), GLFW_CURSOR, m_mouseLocked ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
	}
	if (m_mouseLocked) glfwSetCursorPos(Window.getWindowContext(), windowCenterWidth, windowCenterHeight);

	// Handle camera zoom.
	m_cameraDistance = Math::clamp(m_cameraDistance + (cameraInput.z() * s_cameraZoomSpeed), s_cameraDistanceBounds.x(), s_cameraDistanceBounds.y());

	// Get camera rotation vector.
	if (cameraInput.lengthSquared() > 0) {
		getRotation().x() = Math::clamp(getRotation().x() + cameraInput.y() * s_cameraSensitivity * 0.5f * 0.01f, -s_cameraVerticalViewAngle, s_cameraVerticalViewAngle);
		getRotation().y() = Math::mod(getRotation().y() + cameraInput.x() * s_cameraSensitivity * 0.01f, Math::TAU);
	}
	const Vector3 cameraDirection(
		Math::sin(getRotation().y()) * Math::cos(getRotation().x()),
		-Math::sin(getRotation().x()),
		Math::cos(getRotation().y()) * Math::cos(getRotation().x())
	);

	// Update projection.
	Draw.setProjection(
		Matrix4::perspective(
			3.14159265f * 0.5f,
			(float)Window.getWidth() / (float)Window.getHeight(),
			0.1f, float(voxelChunkViewDistance * (Math::max(voxelChunkSizeX, voxelChunkSizeZ) + 1))
		)
	);

	// Get camera position.
	const Vector3 cameraOrigin = playerObject->getPosition() + s_cameraOffset;
	const Vector3 cameraTargetOffset = playerObject->getSafeVelocity() * s_cameraFollowVelocityFactor;
	setPosition(getPosition().lerp(cameraOrigin + cameraTargetOffset, s_cameraFollowStrength * Time.delta()));
	const Vector3 cameraPosition = (cameraDirection * m_cameraDistance);

	// Set camera view.
	Draw.setView(Matrix4::lookAt(getPosition() + cameraPosition, getPosition()));
}

void CameraObject::onDraw() {
	// Debug view.
	static bool lineMode = false;
	if (Input.getKeyDown(GLFW_KEY_APOSTROPHE)) {
		lineMode = !lineMode;
		if (lineMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}