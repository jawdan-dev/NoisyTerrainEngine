#pragma once
#include <Engine/NoisyTerrain.hpp>

class CameraObject : public GameObject {
private:
	float m_cameraDistance = 1.0f;
	bool m_mouseLocked = false;

public:
	void onInitialization() override;
	void onProcess() override;
	void onDraw() override;
};