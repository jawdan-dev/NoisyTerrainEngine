#include "VoxelLocation.hpp"

#include <Engine/Voxel/VoxelInformation.hpp>
#include <Engine/Voxel/Types/ChunkLocation.hpp>

VoxelLocation::VoxelLocation() : VoxelLocation(0, 0, 0) {}
VoxelLocation::VoxelLocation(const VoxelInt x, const VoxelInt y, const VoxelInt z) :
	m_x(x), m_y(y), m_z(z) {}

const VoxelLocation VoxelLocation::getRelativeLocation() const {
	return VoxelLocation(
		mod(m_x, voxelChunkSizeX),
		mod(m_y, voxelChunkSizeY),
		mod(m_z, voxelChunkSizeZ)
	);
}

const VoxelLocation VoxelLocation::operator +(const VoxelLocation& other) const {
	return VoxelLocation(
		m_x + other.m_x,
		m_y + other.m_y,
		m_z + other.m_z
	);
}
const VoxelLocation VoxelLocation::operator -(const VoxelLocation& other) const {
	return VoxelLocation(
		m_x - other.m_x,
		m_y - other.m_y,
		m_z - other.m_z
	);
}
const VoxelLocation VoxelLocation::operator *(const VoxelLocation& other) const {
	return VoxelLocation(
		m_x * other.m_x,
		m_y * other.m_y,
		m_z * other.m_z
	);
}

const bool VoxelLocation::operator <(const VoxelLocation& other) const {
	if (m_x != other.m_x) return m_x < other.m_x;
	if (m_y != other.m_y) return m_y < other.m_y;
	return m_z < other.m_z;
}
const bool VoxelLocation::operator ==(const VoxelLocation& other) const {
	return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z;
}
const bool VoxelLocation::operator !=(const VoxelLocation& other) const {
	return m_x != other.m_x || m_y != other.m_y || m_z != other.m_z;
}

VoxelLocation::operator ChunkLocation() const {
	return ChunkLocation(
		(m_x - mod(m_x, voxelChunkSizeX)) / voxelChunkSizeX,
		(m_z - mod(m_z, voxelChunkSizeZ)) / voxelChunkSizeZ
	);
}