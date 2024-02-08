#include <JLib/JLib.hpp>
using namespace JLib;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
	printf("Wassup, 'Matrix.\n");

	// GLFW init.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// Create window.
	GLFWwindow* const window = glfwCreateWindow(640, 360, "Title", NULL, NULL);
	if (!window) {
		return EXIT_FAILURE;
	}

	// Test loop.
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}