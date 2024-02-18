#include "Window.hpp"

J_SINGLETON_DEF(WindowManager);

WindowManager::WindowManager() :
	m_window(nullptr),
	m_x(0), m_y(0), m_width(0), m_height(0),
	m_time(), m_input(), m_draw(), m_entityManager() {
	// GLFW init.
	if (!glfwInit()) J_ERROR_EXIT("Window.cpp: Failed to initialize window.\n");

	// GL hints.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// Window Hints.
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	// Create window.
	m_window = glfwCreateWindow(640, 360, "...", NULL, NULL);
	if (m_window == nullptr) J_ERROR_EXIT("Window.cpp: Failed to initialize window.\n");
	glfwMakeContextCurrent(m_window);

	// Reset Hints.
	glfwDefaultWindowHints();

	// Initialize GLEW.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) J_ERROR_EXIT("Window.cpp: Failed to initialize GLEW for window.\n");

	// Get information.
	glfwGetWindowPos(m_window, &m_x, &m_y);
	glfwGetWindowSize(m_window, &m_width, &m_height);

	// Set config.
	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

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
		glfwMakeContextCurrent(_);
		glViewport(0, 0, width, height);
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

	// Show window.
	glfwShowWindow(m_window);
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

	// Update time.
	m_time.updateTime();

	// Bind singletons.
	J_SINGLETON_SET(WindowManager, this);
	J_SINGLETON_SET(TimeManager, &m_time);
	J_SINGLETON_SET(InputManager, &m_input);
	J_SINGLETON_SET(DrawManager, &m_draw);

	// Set context.
	glfwMakeContextCurrent(m_window);

	// Clear buffer.
	m_draw.clear();

	// Handle entities.
	m_entityManager.processAll();
	m_entityManager.drawAll();

	// Draw.
	m_draw.drawAll();

	// Process inputs.
	m_input.process();

	// Swap buffers.
	glfwSwapBuffers(m_window);
	return true;
}