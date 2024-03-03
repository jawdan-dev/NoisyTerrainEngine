#include "EntityManager.hpp"

EntityManager::EntityManager() :
	m_hasUnreadiedEntities(false), m_entities() {}
EntityManager::~EntityManager() {}

void EntityManager::processAll() {
	// Check if any entities are to be readied.
	if (m_hasUnreadiedEntities) {
		// Ready all entities.
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			Entity* const entity = *it;
			// Make sure entity has been readied.
			if (!entity->m_readied) {
				entity->onReady();
				entity->m_readied = true;
			}
		}

		// Update details.
		m_hasUnreadiedEntities = false;
	}

	// Process all (legacy style).
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
		(*it)->onProcess();
}
void EntityManager::drawAll() {
	// Draw all.
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
		(*it)->onDraw();
}

void EntityManager::addEntity(Entity* const entity) {
	// Add entity.
	m_hasUnreadiedEntities = true;
	m_entities.emplace(entity);
}
void EntityManager::removeEntity(Entity* const entity) {
	// Remove entity.
	m_entities.erase(entity);
}
void EntityManager::deleteAll() {
	// Delete and remove all.
	while (m_entities.size() > 0) {
		auto it = m_entities.begin();

		// Delete.
		delete* it;

		// Remove.
		m_entities.erase(it);
	}
}
