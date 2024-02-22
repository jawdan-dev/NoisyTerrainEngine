#include <NoisyTerrain/NoisyTerrain.hpp>

#include <Entities/CameraEntity.hpp>
#include <Entities/VoxelEntity.hpp>

int main() {
	printf("Wassup, 'Matrix.\n");

	// Create window.
	WindowManager window;

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