#pragma once
#include <Engine/Collision/CollisionBase.hpp>

class ColliderSphere : public ColliderBase {
private:
	Vector3 m_position;
	float m_radius;

public:
	ColliderSphere(const Vector3& position, const float radius);

public:
	J_GETTER_DIRECT(getPosition, m_position, Vector3&);
	J_GETTER_DIRECT(getRadius, m_radius, float);

public:
	J_SETTER_DIRECT(setPosition, const Vector3& position, m_position = position);
	J_SETTER_DIRECT(setRadius, const float radius, m_radius = radius);

public:
	void draw(const Vector3& color) const override;
};