#pragma once
#include <NoisyTerrain/NoisyTerrain.hpp>

class CameraEntity : public Entity {
public:
	static Vector3 activeCameraPosition;

private:
	Vector3 position = Vector3(0, 0, 1), rotation = Vector3(0, 3.14159265f, 0);

public:
	void onProcess() {
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
			const float moveSpeed = Input.getKey(GLFW_KEY_LEFT_SHIFT) ? 50.0f : Input.getKey(GLFW_KEY_LEFT_ALT) ? 5.0f : 15.0f;
			position.x() += ((cosf(rotation.y()) * moveAmount.x()) + (-sinf(rotation.y()) * moveAmount.z())) * moveSpeed * Time.delta();;
			position.y() += moveAmount.y() * moveSpeed * Time.delta();
			position.z() += ((-sinf(rotation.y()) * moveAmount.x()) + (-cosf(rotation.y()) * moveAmount.z())) * moveSpeed * Time.delta();;
			activeCameraPosition = position;

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
		Draw.setView(
			Matrix4::rotation(-rotation) * Matrix4::translation(-position)
		);
	}

	void onDraw() {
		// Debug view.
		static bool lineMode = false;
		if (Input.getKeyDown(GLFW_KEY_APOSTROPHE)) {
			lineMode = !lineMode;
			if (lineMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
};

Vector3 CameraEntity::activeCameraPosition;