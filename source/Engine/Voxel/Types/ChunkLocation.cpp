#include "ChunkLocation.hpp"

#include <Engine/Voxel/VoxelInformation.hpp>
#include <Engine/Voxel/Types/VoxelLocation.hpp>

ChunkLocation::ChunkLocation() : ChunkLocation(0, 0) {}
ChunkLocation::ChunkLocation(const VoxelInt x, const VoxelInt z) :
	m_x(x), m_z(z) {}

const ChunkLocation ChunkLocation::operator +(const ChunkLocation& other) const {
	return ChunkLocation(
		m_x + other.m_x,
		m_z + other.m_z
	);
}
const ChunkLocation ChunkLocation::operator -(const ChunkLocation& other) const {
	return ChunkLocation(
		m_x - other.m_x,
		m_z - other.m_z
	);
}
const ChunkLocation ChunkLocation::operator *(const ChunkLocation& other) const {
	return ChunkLocation(
		m_x * other.m_x,
		m_z * other.m_z
	);
}

const bool ChunkLocation::operator <(const ChunkLocation& other) const {
	if (m_x != other.m_x) return m_x < other.m_x;
	return m_z < other.m_z;
}
const bool ChunkLocation::operator ==(const ChunkLocation& other) const {
	return m_x == other.m_x && m_z == other.m_z;
}
const bool ChunkLocation::operator !=(const ChunkLocation& other) const {
	return m_x != other.m_x || m_z != other.m_z;
}

ChunkLocation::operator VoxelLocation() const {
	return VoxelLocation(
		m_x * voxelChunkSizeX,
		0,
		m_z * voxelChunkSizeZ
	);
}