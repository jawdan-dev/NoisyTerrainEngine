#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Entity/Entity.hpp>
#include <NoisyTerrain/Window/Window.hpp>

int main() {
	printf("Wassup, 'Matrix.\n");

	// Create window.
	WindowManager window;

	// Process.
	while (window.process());

	// Cleanup.
	window.getEntityManager()->deleteAll();

	// Successful exit.
	return EXIT_SUCCESS;
}