#include "PlayerObject.hpp"

EntityBody PlayerObject::s_entityBody = EntityBody("Player");

PlayerObject::PlayerObject() :
	EntityObjectBase(ColliderCube(Vector3(-0.4f, 0.0f, -0.4f), Vector3(0.8f, 0.8f, 0.8f)), &s_entityBody) {}

void PlayerObject::onInitialization() {
	// Initialize data.
	setPosition(Vector3(64, 130, 64));
}

void PlayerObject::onProcess() {
	// Player config.
	static constexpr const float s_horizontalMovementSpeed = 8.0f;
	static constexpr const float s_jumpImpulse = 8.0f;

	// Get camera.
	const CameraObject* const camera = Window.getGameObjectManager()->getFirstGameObjectOfType<CameraObject>();
	if (camera == nullptr) return;

	// Get input.
	const Vector3 moveAmount(
		(Input.getKey(GLFW_KEY_D) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_A) ? 1.0f : 0.0f),
		(isOnGround() && Input.getKeyDown(GLFW_KEY_SPACE)) ? 1.0f : 0.0f,
		(Input.getKey(GLFW_KEY_W) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_S) ? 1.0f : 0.0f)
	);

	// Get horizontal movement factor.
	const Vector2 horizontalMovement = Vector2(
		(Math::cos(camera->getRotation().y()) * moveAmount.x()) + (-Math::sin(camera->getRotation().y()) * moveAmount.z()),
		(-Math::sin(camera->getRotation().y()) * moveAmount.x()) + (-Math::cos(camera->getRotation().y()) * moveAmount.z())
	) * s_horizontalMovementSpeed * (Input.getKey(GLFW_KEY_LEFT_SHIFT) ? 5.0f : 1.0f);

	// Set velocity.
	getVelocity().x() = horizontalMovement.x();
	if (moveAmount.y()) getVelocity().y() = moveAmount.y() * s_jumpImpulse;
	getVelocity().z() = horizontalMovement.y();

	// Process base.
	EntityObjectBase::onProcess();
};
