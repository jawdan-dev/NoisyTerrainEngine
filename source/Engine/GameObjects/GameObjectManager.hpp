#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/GameObjects/GameObject.hpp>

class GameObjectManager {
private:
	bool m_hasUninitializedGameObjects;
	Set<GameObject*> m_gameObjects;

public:
	GameObjectManager();
	GameObjectManager(const GameObjectManager& other) = delete;
	~GameObjectManager();

public:
	void processAllGameObject();
	void drawAllGameObject();

public:
	template <typename T, J_ENABLE_IF_BASE_OF(T, GameObject)> T* const getFirstGameObjectOfType() const;
	template <typename T, J_ENABLE_IF_BASE_OF(T, GameObject)> const List<T*> getGameObjectsOfType() const;

public:
	void addGameObject(GameObject* const entity);
	void removeGameObject(GameObject* const entity);
	void deleteAllGameObject();
};

#include "GameObjectManager.ipp"