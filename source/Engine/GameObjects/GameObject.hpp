#pragma once
#include <Engine/Core/Core.hpp>

class GameObject {
	friend class GameObjectManager;

private:
	bool m_initialized;
	Vector3 m_position, m_rotation;

public:
	GameObject();
	virtual ~GameObject();

public:
	J_GETTER_DIRECT(getPosition, m_position, Vector3&);
	J_GETTER_DIRECT_MUT(getPosition, m_position, Vector3&);
	J_GETTER_DIRECT(getRotation, m_rotation, Vector3&);
	J_GETTER_DIRECT_MUT(getRotation, m_rotation, Vector3&);
	J_GETTER_DIRECT(getTransformMatrix, Matrix4::rotation(m_rotation)* Matrix4::translation(m_position), Matrix4);
	J_GETTER_DIRECT(getInverseTransformMatrix, Matrix4::rotation(-m_rotation)* Matrix4::translation(-m_position), Matrix4);

public:
	J_SETTER_DIRECT(setPosition, const Vector3& position, m_position = position);
	J_SETTER_DIRECT(setRotation, const Vector3& rotation, m_rotation = rotation);

public:
	virtual void onInitialization() {}
	virtual void onProcess() {}
	virtual void onDraw() {}
};