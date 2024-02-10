#include "Input.hpp"

InputManager::InputManager() :
	m_mouseX(0), m_mouseY(0), m_mouseScrollX(0), m_mouseScrollY(0),
	m_keyDown(), m_keyRepeat(), m_keyPressed(), m_keyUp(),
	m_mouseDown(), m_mousePressed(), m_mouseUp() {}
InputManager::~InputManager() {}

void InputManager::process() {
	// Key clear.
	m_keyDown.clear();
	m_keyRepeat.clear();
	m_keyUp.clear();

	// Mouse clear.
	m_mouseDown.clear();
	m_mouseUp.clear();
}

const bool InputManager::getKeyDown(const GLenum key) {
	return m_keyDown.find(key) != m_keyDown.end();
}
const bool InputManager::getKeyRepeat(const GLenum key) {
	return m_keyRepeat.find(key) != m_keyRepeat.end();
}
const bool InputManager::getKey(const GLenum key) {
	return m_keyPressed.find(key) != m_keyPressed.end();
}
const bool InputManager::getKeyUp(const GLenum key) {
	return m_keyUp.find(key) != m_keyUp.end();
}

const bool InputManager::getMouseButtonDown(const GLenum key) {
	return m_mouseDown.find(key) != m_mouseDown.end();
}
const bool InputManager::getMouseButton(const GLenum key) {
	return m_mousePressed.find(key) != m_mousePressed.end();
}
const bool InputManager::getMouseButtonUp(const GLenum key) {
	return m_mouseUp.find(key) != m_mouseUp.end();
}
const int InputManager::getMouseX() {
	return m_mouseX;
}
const int InputManager::getMouseY() {
	return m_mouseY;
}
const int InputManager::getMouseScrollX() {
	return m_mouseScrollX;
}
const int InputManager::getMouseScrollY() {
	return m_mouseScrollY;
}

void InputManager::setKey(const GLenum key, const GLenum action) {
	switch (action) {
		case GLFW_PRESS: {
			m_keyDown.emplace(key);
			m_keyPressed.emplace(key);
		}
		case GLFW_REPEAT: {
			m_keyRepeat.emplace(key);
		} break;
		case GLFW_RELEASE: {
			m_keyPressed.erase(key);
			m_keyUp.emplace(key);
		} break;
	}
}
void InputManager::setMouseButton(const GLenum key, const GLenum action) {
	switch (action) {
		case GLFW_PRESS: {
			m_mouseDown.emplace(key);
			m_mousePressed.emplace(key);
		} break;
		case GLFW_RELEASE: {
			m_mousePressed.erase(key);
			m_mouseUp.emplace(key);
		} break;
	}
}
void InputManager::setMousePosition(const int x, const int y) {
	m_mouseX = x;
	m_mouseY = y;
}
void InputManager::setMouseScroll(const int x, const int y) {
	m_mouseScrollX = x;
	m_mouseScrollY = y;
}