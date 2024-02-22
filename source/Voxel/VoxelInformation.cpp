#include "VoxelInformation.hpp"

const Vector3 getVoxelColor(const VoxelID voxelID) {
	switch (voxelID) {
		case VoxelID::Bedrock: return Vector3(0.1f, 0.1f, 0.1f);
		case VoxelID::Stone: return Vector3(0.5f, 0.5f, 0.5f);

		case VoxelID::Grass: return Vector3(0.0f, 1.0f, 0.5f);
		case VoxelID::Dirt: return Vector3(0.5f, 0.25f, 0.1f);

		case VoxelID::Water: return Vector3(0.0f, 0.5f, 1.0f);

		case VoxelID::Wood: return Vector3(1.0f, 0.25f, 0.1f);
		case VoxelID::Leaf: return Vector3(0.1f, 0.8f, 0.45f);
	}

	// No color found.
	return Vector3(1.0f, 0.0f, 1.0f);
}
