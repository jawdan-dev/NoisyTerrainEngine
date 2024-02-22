#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Layer/LayerSegment.hpp>

class Layer {
private:
	List<LayerSegment> m_layerData;

public:
	Layer();
	Layer(const Layer& other);
	~Layer();

public:
	const VoxelID getVoxel(const VoxelInt x, const VoxelInt z) const;

public:
	const bool setVoxel(const VoxelInt x, const VoxelInt z, const VoxelID id);

public:
	void compress();

public:
	const bool operator <(const Layer& other) const;
};