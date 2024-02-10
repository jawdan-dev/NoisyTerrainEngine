#include "Window.hpp"

WindowManager::WindowManager() {
	// GLFW init.
	if (!glfwInit()) J_ERROR_EXIT("Window.cpp: Failed to initialize window.\n");

	// Set GL hints.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// Create window.
	m_window = glfwCreateWindow(640, 360, "...", NULL, NULL);
	if (m_window == nullptr) J_ERROR_EXIT("Window.cpp: Failed to initialize window.\n");
	glfwMakeContextCurrent(m_window);

	// Initialize GLEW.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) J_ERROR_EXIT("Window.cpp: Failed to initialize GLEW for window.\n");

	// Get information.
	glfwGetWindowPos(m_window, &m_x, &m_y);
	glfwGetWindowSize(m_window, &m_width, &m_height);

	// Set callbacks.
#pragma region callbacks

	glfwSetWindowUserPointer(m_window, this);

	glfwSetWindowPosCallback(m_window, [](GLFWwindow* const _, const int x, const int y) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_x = x;
		window->m_y = y;
		});
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* const _, const int width, const int height) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_width = width;
		window->m_height = height;
		});

	glfwSetKeyCallback(m_window, [](GLFWwindow* const _, const int key, const int scancode, const int action, const int mods) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_input.setKey(key, action);
		});
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* const _, const int button, const int action, const int mods) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_input.setMouseButton(button, action);
		});
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* const _, const double mouseX, const double mouseY) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_input.setMousePosition(mouseX, mouseY);
		});
	glfwSetScrollCallback(m_window, [](GLFWwindow* const _, const double xOffset, const double yOffset) {
		WindowManager* const window = (WindowManager*)glfwGetWindowUserPointer(_);
		window->m_input.setMouseScroll(xOffset, yOffset);
		});

#pragma endregion
}
WindowManager::~WindowManager() {
	// Delete window.
	glfwDestroyWindow(m_window);

	// A strong choice, but we move.
	glfwTerminate();
}

const bool WindowManager::process() {
	// Handle events.
	glfwPollEvents();
	if (glfwWindowShouldClose(m_window)) return false;

	// Bind singletons.
	J_SINGLETON_SET(WindowManager, this);
	J_SINGLETON_SET(InputManager, &m_input);

	// Set context.
	glfwMakeContextCurrent(m_window);

	// Clear buffer.
	// TODO: Draw reset.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Handle entities.
	m_entityManager.processAll();
	m_entityManager.drawAll();

	// Draw.
	// TODO: Draw draw.

	// Swap buffers.
	glfwSwapBuffers(m_window);
	return true;
}