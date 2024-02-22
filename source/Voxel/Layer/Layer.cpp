#include "Layer.hpp"

Layer::Layer() : m_layerData() {
	m_layerData.emplace_back(VoxelID::None, voxelChunkSizeX * voxelChunkSizeZ);
}
Layer::Layer(const Layer& other) :
	m_layerData(other.m_layerData) {}
Layer::~Layer() {}

const VoxelID Layer::getVoxel(const VoxelInt x, const VoxelInt z) const {
	// Get index.
	VoxelLayerSegmentInt index = x + (z * voxelChunkSizeZ);
	if (index > (voxelChunkSizeX * voxelChunkSizeZ)) return VoxelID::None;

	// Find voxel ID.
	for (size_t i = 0; i < m_layerData.size(); i++) {
		// Check if in segment.
		if (index < m_layerData[i].m_count)
			return m_layerData[i].m_id;

		// Offset through index.
		index -= m_layerData[i].m_count;
	}

	// ID not found.
	return VoxelID::None;
}

const bool Layer::setVoxel(const VoxelInt x, const VoxelInt z, const VoxelID id) {
	// Get index.
	VoxelLayerSegmentInt index = x + (z * voxelChunkSizeZ);
	if (index > (voxelChunkSizeX * voxelChunkSizeZ)) return false;

	// Find associated segment.
	for (auto it = m_layerData.begin(); it != m_layerData.end(); it++) {
		// Check if in segment.
		if (index < it->m_count) {
			// Ignore if same.
			if (id == it->m_id) return false;

			// Split segment.
			VoxelLayerSegmentInt
				splitLeft = index,
				splitRight = it->m_count - (index + 1); // 5v2 -> 0 1 x 3 4

			if (splitLeft + splitRight == 0) {
				// Replace segment.
				it->m_id = id;
			} else if (splitLeft == 0) {
				// Insert left.
				it->m_count = splitRight;
				m_layerData.insert(it, LayerSegment(id, 1));
			} else if (splitRight == 0) {
				// Insert right.
				it->m_count = splitLeft;
				m_layerData.insert(it + 1, LayerSegment(id, 1));
			} else {
				// Normal split insert.
				it->m_count = splitLeft;
				m_layerData.insert(
					m_layerData.insert(it + 1, LayerSegment(id, 1)) + 1,
					LayerSegment(it->m_id, splitRight)
				);
			}

			// Voxel placed.
			return true;
		}

		// Offset through index.
		index -= it->m_count;
	}

	// No voxel placed.
	return false;
}

void Layer::compress() {
	// Remove 0-length segments.
	for (auto it = m_layerData.begin(); it != m_layerData.end();) {
		if (it->m_count == 0) it = m_layerData.erase(it);
		else it++;
	}

	// Merge segments with the same id.
	for (size_t i = 1; i < m_layerData.size();) {
		// Skip if same.
		if (m_layerData[i - 1].m_id != m_layerData[i].m_id) {
			i++;
			continue;
		}

		// Merge segments.
		m_layerData[i - 1].m_count += m_layerData[i].m_count;
		m_layerData.erase(m_layerData.begin() + i);
	}

	// Error check (for my own sanity).
	size_t totalCount = 0;
	for (auto it = m_layerData.begin(); it != m_layerData.end(); it++) {
		totalCount += it->m_count;
	}
	if (totalCount != voxelChunkSizeX * voxelChunkSizeZ)
		J_ERROR("VoxelLayer.cpp: Voxel layer found with invalid data count of %zu\n", totalCount);
}

const bool Layer::operator <(const Layer& other) const {
	// Check size.
	if (m_layerData.size() != other.m_layerData.size())
		return m_layerData.size() < other.m_layerData.size();

	// Check contents.
	for (size_t i = 0; i < m_layerData.size(); i++) {
		// Compare segment lengths.
		if (m_layerData[i].m_count != other.m_layerData[i].m_count)
			return m_layerData[i].m_count < other.m_layerData[i].m_count;

		// Compare segment IDs.
		if (m_layerData[i].m_id != other.m_layerData[i].m_id)
			return m_layerData[i].m_id < other.m_layerData[i].m_id;
	}

	// The layers are the same.
	return false;
}