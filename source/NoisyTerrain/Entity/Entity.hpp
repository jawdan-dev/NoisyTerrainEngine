#pragma once
#include <NoisyTerrain/Core/Core.hpp>

class Entity {
	friend class EntityManager;

private:
	bool m_readied;
	Vector3 m_position, m_rotation;

public:
	Entity();
	~Entity();

public:
	J_GETTER_DIRECT_MUT(getPosition, m_position, Vector3&);
	J_GETTER_DIRECT_MUT(getRotation, m_rotation, Vector3&);

public:
	virtual void onReady() {}
	virtual void onProcess() {}
	virtual void onDraw() {}
};