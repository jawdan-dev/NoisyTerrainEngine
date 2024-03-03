#pragma once
#include <Engine/Core/Core.hpp>

#include <Voxel/Layer/Layer.hpp>

class LayerManager;

class LayerReference {
private:
	LayerManager* m_layerManager;
	const Layer* m_layer;

public:
	LayerReference();
	LayerReference(LayerManager* const layerManager, const Layer* const layer);
	LayerReference(const LayerReference& other);
	~LayerReference();

public:
	J_GETTER_DIRECT(isValid, m_layerManager != nullptr && m_layer != nullptr, bool);
	const VoxelID getVoxel(const VoxelInt x, const VoxelInt z) const;
	J_GETTER_DIRECT(getVoxelLayer, m_layer, Layer* const);
	J_GETTER_DIRECT_MUT(getVoxelLayerManager, m_layerManager, LayerManager* const);

public:
	LayerReference& operator =(const LayerReference& other);
};