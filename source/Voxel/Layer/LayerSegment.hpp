#pragma once
#include <Engine/Core/Core.hpp>

#include <Voxel/VoxelInformation.hpp>

typedef uint32_t VoxelLayerSegmentInt;
struct LayerSegment {
	VoxelID m_id;
	VoxelLayerSegmentInt m_count;

	LayerSegment();
	LayerSegment(const VoxelID id, const VoxelLayerSegmentInt count);
};