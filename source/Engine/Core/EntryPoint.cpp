#include <Engine/NoisyTerrain.hpp>

#include <GameObjects/CameraObject.hpp>
#include <GameObjects/VoxelManagerObject.hpp>
#include <GameObjects/PlayerObject.hpp>

int main() {
	printf("Wassup, 'Matrix.\n");

	// Create window.
	WindowManager window;
	window.setClearColor(0.45f, 0.6f, 1.0f);

	// TODO: Scenes?
	// Environment setup.
	GameObjectManager& gom = *window.getGameObjectManager();
	gom.addGameObject(new CameraObject);
	gom.addGameObject(new VoxelManagerObject);
	gom.addGameObject(new PlayerObject);

	// Process.
	while (window.process());

	// Cleanup.
	window.getGameObjectManager()->deleteAllGameObject();

	// Successful exit.
	return EXIT_SUCCESS;
}