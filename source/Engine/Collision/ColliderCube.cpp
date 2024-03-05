#include "ColliderCube.hpp"

#include <Engine/Draw/Draw.hpp>

ColliderCube::ColliderCube(const Vector3& position, const Vector3& size) :
	ColliderBase(ColliderType::Cube),
	m_position(position), m_size(size.abs()) {}

void ColliderCube::draw(const Vector3& color) const {
	// Model.
	static bool modelLoaded = false;
	static Model model;
	if (!modelLoaded) {
		// Load model.
		model.lock();
		model.load("UnitCube.obj");
		model.upload();
		model.unlock();
		// Update details.
		modelLoaded = true;
	}

	// Draw.
	InstanceData instanceData(&ColliderBase::m_debugShader);
	instanceData.setData("i_position", m_position + (m_size * 0.5f));
	instanceData.setData("i_scale", m_size);
	instanceData.setData("i_color", color);
	Draw.draw(model, instanceData);
}