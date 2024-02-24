#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Types/VoxelInt.hpp>

//////////////////////////////////////////////////////////////////////////////////////

// Holy constants #1-#3: Voxel chunk size.
static constexpr VoxelInt voxelChunkSizeX = 16, voxelChunkSizeY = 256, voxelChunkSizeZ = voxelChunkSizeX;

// Holy constants #4-#5: Chunk generation speeds.
static constexpr VoxelInt voxelChunkBatchSize = 10, voxelChunkViewDistance = 10;

// Holy constants #6: Chunk upload config.
static constexpr size_t chunkUploadSpeed = 4000;

// Demonic constraints #1-#2: World boundaries.
static constexpr VoxelInt worldMaxHorizontalBound = 50;

//////////////////////////////////////////////////////////////////////////////////////

typedef uint8_t VoxelIDType;
enum class VoxelID : VoxelIDType {
	None = 0,

	// All the blocks.
	Air = None,

	Bedrock, Stone,
	Grass, Dirt,
	Water,
	Wood, Leaf,

	// Not so block.
	Last
};

const Vector3 getVoxelColor(const VoxelID voxelID);

//////////////////////////////////////////////////////////////////////////////////////