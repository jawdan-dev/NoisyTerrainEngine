#pragma once
#include <NoisyTerrain/Core/Core.hpp>

class InputManager {
private:
	int m_mouseX, m_mouseY, m_mouseScrollX, m_mouseScrollY;
	Set<GLenum>
		m_keyDown, m_keyRepeat, m_keyPressed, m_keyUp,
		m_mouseDown, m_mousePressed, m_mouseUp;

public:
	InputManager();
	InputManager(const InputManager& other) = delete;
	~InputManager();

public:
	void process();

public:
	const bool getKeyDown(const GLenum key);
	const bool getKeyRepeat(const GLenum key);
	const bool getKey(const GLenum key);
	const bool getKeyUp(const GLenum key);

	const bool getMouseButtonDown(const GLenum key);
	const bool getMouseButton(const GLenum key);
	const bool getMouseButtonUp(const GLenum key);
	const int getMouseX();
	const int getMouseY();
	const int getMouseScrollX();
	const int getMouseScrollY();

public:
	void setKey(const GLenum key, const GLenum action);
	void setMouseButton(const GLenum key, const GLenum action);
	void setMousePosition(const int x, const int y);
	void setMouseScroll(const int x, const int y);
};

J_SINGLETON(InputManager)
#define Input J_SINGLETON_GET(InputManager)