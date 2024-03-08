#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Time/Time.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Draw/Draw.hpp>

#include <Engine/GameObjects/GameObjectManager.hpp>

class WindowManager {
	J_SINGLETON(WindowManager)
#	define Window J_SINGLETON_GET(WindowManager)

private:
	GLFWwindow* m_window;
	int m_x, m_y, m_width, m_height;
	bool m_isFocused;

	ThreadPoolManager m_threadPool;
	TimeManager m_time;
	InputManager m_input;
	DrawManager m_draw;
	GameObjectManager m_gameObjectManager;

public:
	WindowManager();
	WindowManager(const WindowManager& other) = delete;
	~WindowManager();

public:
	J_GETTER_DIRECT_MUT(getWindowContext, m_window, GLFWwindow* const);
	J_GETTER_DIRECT(getX, m_x, int);
	J_GETTER_DIRECT(getY, m_y, int);
	J_GETTER_DIRECT(getWidth, m_width, int);
	J_GETTER_DIRECT(getHeight, m_height, int);
	J_GETTER_DIRECT(isFocused, m_isFocused, bool);

	J_GETTER_DIRECT_MUT(getThreadPool, &m_threadPool, ThreadPoolManager* const);
	J_GETTER_DIRECT_MUT(getTimeManager, &m_time, TimeManager* const);
	J_GETTER_DIRECT_MUT(getInputManager, &m_input, InputManager* const);
	J_GETTER_DIRECT_MUT(getDrawManager, &m_draw, DrawManager* const);
	J_GETTER_DIRECT_MUT(getGameObjectManager, &m_gameObjectManager, GameObjectManager* const);

public:
	J_SETTER_DIRECT(setTitle, const char* title, glfwSetWindowTitle(m_window, title));
	J_SETTER_DIRECT(setClearColor, ARGS(const float r, const float g, const float b, const float a = 1.0f), glfwMakeContextCurrent(m_window); glClearColor(r, g, b, a));

public:
	const bool process();
};
