#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Types/VoxelInt.hpp>

class VoxelLocation;

class ChunkLocation {
private:
	VoxelInt m_x, m_z;

public:
	ChunkLocation();
	ChunkLocation(const VoxelInt x, const VoxelInt z);

public:
	J_GETTER_DIRECT_MUT(x, m_x, VoxelInt&);
	J_GETTER_DIRECT_MUT(z, m_z, VoxelInt&);

public:
	J_GETTER_DIRECT(x, m_x, VoxelInt);
	J_GETTER_DIRECT(z, m_z, VoxelInt);

public:
	const ChunkLocation operator +(const ChunkLocation& other) const;
	const ChunkLocation operator -(const ChunkLocation& other) const;
	const ChunkLocation operator *(const ChunkLocation& other) const;

	const bool operator <(const ChunkLocation& other) const;

public:
	operator VoxelLocation() const;
};
