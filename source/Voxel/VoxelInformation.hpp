#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Types/VoxelInt.hpp>

//////////////////////////////////////////////////////////////////////////////////////

// Holy constants #1-#3: Voxel chunk size.
static constexpr VoxelInt voxelChunkSizeX = 16, voxelChunkSizeY = 256, voxelChunkSizeZ = voxelChunkSizeX;

// Holy constants #4-#6: Chunk generation speeds.
static constexpr VoxelInt voxelChunkBatchSize = 32, voxelChunkViewDistance = 16;

// Holy constants #6: Chunk upload config.
static constexpr size_t chunkUploadSpeed = 4000;

// Demonic constraints #1-#2: World boundaries.
static constexpr VoxelInt worldMaxHorizontalBound = 50;

//////////////////////////////////////////////////////////////////////////////////////

enum class VoxelID : uint8_t {
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

typedef uint8_t VoxelTextureIndex;
const VoxelTextureIndex getVoxelTextureIndexTop(const VoxelID voxelID);
const VoxelTextureIndex getVoxelTextureIndexSide(const VoxelID voxelID);
const VoxelTextureIndex getVoxelTextureIndexBottom(const VoxelID voxelID);

//////////////////////////////////////////////////////////////////////////////////////