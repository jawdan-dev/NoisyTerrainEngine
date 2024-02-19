#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/ThreadPool/ThreadPool.hpp>
#include <NoisyTerrain/Time/Time.hpp>
#include <NoisyTerrain/Input/Input.hpp>
#include <NoisyTerrain/Draw/Draw.hpp>

#include <NoisyTerrain/Entity/EntityManager.hpp>

class WindowManager {
	J_SINGLETON(WindowManager)
#	define Window J_SINGLETON_GET(WindowManager)

private:
	GLFWwindow* m_window;
	int m_x, m_y, m_width, m_height;

	ThreadPoolManager m_threadPool;
	TimeManager m_time;
	InputManager m_input;
	DrawManager m_draw;
	EntityManager m_entityManager;

public:
	WindowManager();
	WindowManager(const WindowManager& other) = delete;
	~WindowManager();

public:
	J_GETTER_DIRECT(getWindowContext, m_window, GLFWwindow* const);
	J_GETTER_DIRECT(getX, m_x, int);
	J_GETTER_DIRECT(getY, m_y, int);
	J_GETTER_DIRECT(getWidth, m_width, int);
	J_GETTER_DIRECT(getHeight, m_height, int);

	J_GETTER_DIRECT_MUT(getThreadPool, &m_threadPool, ThreadPoolManager* const);
	J_GETTER_DIRECT_MUT(getTimeManager, &m_time, TimeManager* const);
	J_GETTER_DIRECT_MUT(getInputManager, &m_input, InputManager* const);
	J_GETTER_DIRECT_MUT(getDrawManager, &m_draw, DrawManager* const);
	J_GETTER_DIRECT_MUT(getEntityManager, &m_entityManager, EntityManager* const);

public:
	const bool process();
};
