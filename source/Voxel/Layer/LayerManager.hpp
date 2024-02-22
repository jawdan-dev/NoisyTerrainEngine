#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Layer/LayerReference.hpp>

class LayerManager {
private:
	Mutex m_layerLock;
	Map<Layer, size_t> m_layers;

public:
	LayerManager();
	LayerManager(const LayerManager& other) = delete;
	~LayerManager();

public:
	const LayerReference createLayerReference(const Layer& layer);
	J_GETTER_DIRECT(getPalleteSize, m_layers.size(), size_t);

public:
	void addLayerReference(const Layer& layer);
	void removeLayerReference(const Layer& layer);

public:
	void lock();
	void unlock();
};