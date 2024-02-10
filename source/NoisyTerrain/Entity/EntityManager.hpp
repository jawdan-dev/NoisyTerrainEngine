#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Entity/Entity.hpp>

class EntityManager {
private:
	bool m_hasUnreadiedEntities;
	Set<Entity*> m_entities;

public:
	EntityManager();
	EntityManager(const EntityManager& other) = delete;
	~EntityManager();

public:
	void processAll();
	void drawAll();

public:
	void addEntity(Entity* const entity);
	void removeEntity(Entity* const entity);
	void deleteAll();
};