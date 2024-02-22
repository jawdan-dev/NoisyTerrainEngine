#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Types/VoxelInt.hpp>

class ChunkLocation;

class VoxelLocation {
private:
	VoxelInt m_x, m_y, m_z;

public:
	VoxelLocation();
	VoxelLocation(const VoxelInt x, const VoxelInt y, const VoxelInt z);

public:
	J_GETTER_DIRECT_MUT(x, m_x, VoxelInt&);
	J_GETTER_DIRECT_MUT(y, m_y, VoxelInt&);
	J_GETTER_DIRECT_MUT(z, m_z, VoxelInt&);

public:
	J_GETTER_DIRECT(x, m_x, VoxelInt);
	J_GETTER_DIRECT(y, m_y, VoxelInt);
	J_GETTER_DIRECT(z, m_z, VoxelInt);

public:
	const VoxelLocation getRelativeLocation() const;

public:
	const VoxelLocation operator +(const VoxelLocation& other) const;
	const VoxelLocation operator -(const VoxelLocation& other) const;
	const VoxelLocation operator *(const VoxelLocation& other) const;

	const bool operator <(const VoxelLocation& other) const;

public:
	operator ChunkLocation() const;
};
