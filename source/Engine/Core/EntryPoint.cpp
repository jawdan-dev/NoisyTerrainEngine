#include <Engine/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>
#include <Entities/VoxelEntity.hpp>

int main() {
	printf("Wassup, 'Matrix.\n");

	// Create window.
	WindowManager window;
	window.setClearColor(0.45f, 0.6f, 1.0f);

	// TODO: Scenes?
	// Environment setup.
	EntityManager& em = *window.getEntityManager();
	em.addEntity(new CameraEntity);
	em.addEntity(new VoxelEntity);

	// Process.
	while (window.process());

	// Cleanup.
	window.getEntityManager()->deleteAll();

	// Successful exit.
	return EXIT_SUCCESS;
}