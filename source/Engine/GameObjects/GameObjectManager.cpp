#include "GameObjectManager.hpp"

GameObjectManager::GameObjectManager() :
	m_hasUninitializedGameObjects(false), m_gameObjects() {}
GameObjectManager::~GameObjectManager() {}

void GameObjectManager::processAllGameObject() {
	// Check if any game objects are to be readied.
	if (m_hasUninitializedGameObjects) {
		// Ready all game objects.
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++) {
			GameObject* const gameObject = *it;
			// Make sure game object has been readied.
			if (!gameObject->m_initialized) {
				gameObject->onInitialization();
				gameObject->m_initialized = true;
			}
		}

		// Update details.
		m_hasUninitializedGameObjects = false;
	}

	// Process all (legacy style).
	for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		(*it)->onProcess();
}
void GameObjectManager::drawAllGameObject() {
	// Draw all.
	for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		(*it)->onDraw();
}

void GameObjectManager::addGameObject(GameObject* const gameObject) {
	// Add game object.
	m_hasUninitializedGameObjects = true;
	m_gameObjects.emplace(gameObject);
}
void GameObjectManager::removeGameObject(GameObject* const gameObject) {
	// Remove game object.
	m_gameObjects.erase(gameObject);
}
void GameObjectManager::deleteAllGameObject() {
	// Delete and remove all.
	while (m_gameObjects.size() > 0) {
		auto it = m_gameObjects.begin();

		// Delete.
		delete* it;

		// Remove.
		m_gameObjects.erase(it);
	}
}
