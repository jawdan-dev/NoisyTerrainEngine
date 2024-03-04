#include "EntityManager.hpp"

template <typename T, J_ENABLE_IF_BASE_OF_()>
T* const EntityManager::getFirstEntityOfType() const {
	// Get first.
	T* entity;
	for (Entity* e : m_entities) {
		// Try cast entity.
		entity = dynamic_cast<T*>(e);
		if (entity == nullptr) continue;

		// Entity found.
		return entity;
	}

	// Entity not found.
	return nullptr;
}
template <typename T, J_ENABLE_IF_BASE_OF_()>
const List<T*> EntityManager::getEntitiesOfType() const {
	// Get entities.
	T* entity;
	List<T*> matchingEntities;
	for (Entity* const e : m_entities) {
		// Try cast entity.
		entity = dynamic_cast<T*>(e);
		if (entity == nullptr) continue;

		// Entity found.
		matchingEntities.emplace_back(entity);
	}

	// Return entities.
	return matchingEntities;
}
