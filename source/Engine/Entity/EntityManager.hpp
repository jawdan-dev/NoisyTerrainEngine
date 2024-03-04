#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Entity/Entity.hpp>

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
	template <typename T, J_ENABLE_IF_BASE_OF(T, Entity)> T* const getFirstEntityOfType() const;
	template <typename T, J_ENABLE_IF_BASE_OF(T, Entity)> const List<T*> getEntitiesOfType() const;

public:
	void addEntity(Entity* const entity);
	void removeEntity(Entity* const entity);
	void deleteAll();
};

#include "EntityManager.ipp"