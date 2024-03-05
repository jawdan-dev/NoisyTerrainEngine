#include "ColliderSphere.hpp"

#include <Engine/Draw/Draw.hpp>

ColliderSphere::ColliderSphere(const Vector3& position, const float radius) :
	ColliderBase(ColliderType::Sphere),
	m_position(position), m_radius(Math::abs(radius)) {}

void ColliderSphere::draw(const Vector3& color) const {
	// Model.
	static bool modelLoaded = false;
	static Model model;
	if (!modelLoaded) {
		// Load model.
		model.lock();
		model.load("UnitSphere.obj");
		model.upload();
		model.unlock();
		// Update details.
		modelLoaded = true;
	}

	// Draw.
	const float diameter = m_radius * 2.0f;
	InstanceData instanceData(&ColliderBase::m_debugShader);
	instanceData.setData("i_position", m_position);
	instanceData.setData("i_scale", Vector3(diameter, diameter, diameter));
	instanceData.setData("i_color", color);
	Draw.draw(model, instanceData);
}