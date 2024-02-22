#include <NoisyTerrain/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>

int main() {
	printf("Wassup, 'Matrix.\n");

	// Create window.
	WindowManager window;

	// TODO: Scenes?
	// Environment setup.
	EntityManager& em = *window.getEntityManager();
	em.addEntity(new CameraEntity);

	// Process.
	while (window.process());

	// Cleanup.
	window.getEntityManager()->deleteAll();

	// Successful exit.
	return EXIT_SUCCESS;
}