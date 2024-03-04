#include "GameObjectManager.hpp"

template <typename T, J_ENABLE_IF_BASE_OF_()>
T* const GameObjectManager::getFirstGameObjectOfType() const {
	// Get first.
	T* gameObject;
	for (GameObject* e : m_gameObjects) {
		// Try cast game object.
		gameObject = dynamic_cast<T*>(e);
		if (gameObject == nullptr) continue;

		// Game object found.
		return gameObject;
	}

	// Game object not found.
	return nullptr;
}
template <typename T, J_ENABLE_IF_BASE_OF_()>
const List<T*> GameObjectManager::getGameObjectsOfType() const {
	// Get game objects.
	T* gameObject;
	List<T*> matchingGameObjects;
	for (GameObject* const e : m_gameObjects) {
		// Try cast game object.
		gameObject = dynamic_cast<T*>(e);
		if (gameObject == nullptr) continue;

		// Game object found.
		matchingGameObjects.emplace_back(gameObject);
	}

	// Return game objects.
	return matchingGameObjects;
}
