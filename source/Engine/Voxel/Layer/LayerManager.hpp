#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Voxel/Layer/LayerReference.hpp>

class LayerManager {
private:
	Mutex m_layerLock;
	Map<Layer, size_t> m_layers;
	bool m_disabled;

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
	void disable();

public:
	void lock();
	void unlock();
};