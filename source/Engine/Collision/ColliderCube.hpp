#pragma once
#include <Engine/Collision/CollisionBase.hpp>

class ColliderCube : public ColliderBase {
private:
	Vector3 m_position, m_size;

public:
	ColliderCube(const Vector3& position, const Vector3& size);

public:
	J_GETTER_DIRECT(getPosition, m_position, Vector3&);
	J_GETTER_DIRECT(getSize, m_size, Vector3&);

public:
	J_SETTER_DIRECT(setPosition, const Vector3& position, m_position = position);
	J_SETTER_DIRECT(setSize, const Vector3& size, m_size = size);

public:
	void draw(const Vector3& color) const override;
};