#pragma once
#include <Engine/NoisyTerrain.hpp>

class CameraObject : public GameObject {
public:
	void onInitialization() override {
		setRotation(Vector3(0, 3.14159265358979f, 0));
	}

	void onProcess() override {
		const Vector3 moveAmount(
			(Input.getKey(GLFW_KEY_D) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_A) ? 1.0f : 0.0f),
			(Input.getKey(GLFW_KEY_SPACE) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_LEFT_CONTROL) ? 1.0f : 0.0f),
			(Input.getKey(GLFW_KEY_W) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_S) ? 1.0f : 0.0f)
		);
		const Vector2 lookAmount(
			(Input.getKey(GLFW_KEY_LEFT) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_RIGHT) ? 1.0f : 0.0f),
			(Input.getKey(GLFW_KEY_UP) ? 1.0f : 0.0f) - (Input.getKey(GLFW_KEY_DOWN) ? 1.0f : 0.0f)
		);

		if (moveAmount.lengthSquared() > 0 || lookAmount.lengthSquared() > 0) {
			// Update position.
			Vector3& position = getPosition(), & rotation = getRotation();

			const float moveSpeed = Input.getKey(GLFW_KEY_LEFT_SHIFT) ? 50.0f : Input.getKey(GLFW_KEY_LEFT_ALT) ? 5.0f : 15.0f;
			position.x() += ((Math::cos(rotation.y()) * moveAmount.x()) + (-Math::sin(rotation.y()) * moveAmount.z())) * moveSpeed * Time.delta();;
			position.y() += moveAmount.y() * moveSpeed * Time.delta();
			position.z() += ((-Math::sin(rotation.y()) * moveAmount.x()) + (-Math::cos(rotation.y()) * moveAmount.z())) * moveSpeed * Time.delta();;

			// Update rotation.
			rotation.x() += lookAmount.y() * Time.delta();
			rotation.y() += lookAmount.x() * Time.delta();
		}

		// Update view.
		Draw.setProjection(
			Matrix4::perspective(
				3.14159265f * 0.5f,
				(float)Window.getWidth() / (float)Window.getHeight(),
				0.1f, 10000.0f
			)
		);
		Draw.setView(getInverseTransformMatrix());
	}

	void onDraw() override {
		// Debug view.
		static bool lineMode = false;
		if (Input.getKeyDown(GLFW_KEY_APOSTROPHE)) {
			lineMode = !lineMode;
			if (lineMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
};