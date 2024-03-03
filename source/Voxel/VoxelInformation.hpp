#pragma once
#include <Engine/Core/Core.hpp>

#include <Voxel/Types/VoxelInt.hpp>

//////////////////////////////////////////////////////////////////////////////////////

// Holy constants #1-#3: Voxel chunk size.
static constexpr VoxelInt voxelChunkSizeX = 28, voxelChunkSizeY = 256, voxelChunkSizeZ = voxelChunkSizeX;

// Holy constants #4-#5: Chunk generation config.
static constexpr VoxelInt voxelChunkBatchSize = 20, voxelChunkViewDistance = 10;
// Holy constants #6: Chunk upload speed.
static constexpr size_t chunkUploadSpeed = 4000;

// Demonic constraints #1: World boundary.
static constexpr VoxelInt worldMaxHorizontalBound = 800 / Math::min(voxelChunkSizeX, voxelChunkSizeZ);

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