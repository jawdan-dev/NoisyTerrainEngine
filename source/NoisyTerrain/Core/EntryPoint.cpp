#include <NoisyTerrain/NoisyTerrain.hpp>

#include <Entities/TestEntity.hpp>

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