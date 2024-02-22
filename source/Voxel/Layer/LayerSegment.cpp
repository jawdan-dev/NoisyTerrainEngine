#include "LayerSegment.hpp"

LayerSegment::LayerSegment() : LayerSegment(VoxelID::None, 0) {}
LayerSegment::LayerSegment(const VoxelID id, const VoxelLayerSegmentInt count) :
	m_id(id), m_count(count) {}