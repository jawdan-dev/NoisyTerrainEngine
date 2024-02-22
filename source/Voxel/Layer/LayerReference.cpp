#include "LayerReference.hpp"

#include <Voxel/Layer/LayerManager.hpp>

LayerReference::LayerReference() : LayerReference(nullptr, nullptr) {}
LayerReference::LayerReference(LayerManager* const layerManager, const Layer* const layer) :
	m_layerManager(layerManager), m_layer(layer) {}
LayerReference::LayerReference(const LayerReference& other) :
	m_layerManager(other.m_layerManager), m_layer(other.m_layer) {
	// Update manager.
	if (isValid()) m_layerManager->addLayerReference(*m_layer);
}
LayerReference::~LayerReference() {
	// Update manager.
	if (isValid()) m_layerManager->removeLayerReference(*m_layer);
}

const VoxelID LayerReference::getVoxel(const VoxelInt x, const VoxelInt y) const {
	// Return voxel ID.
	if (!isValid()) return VoxelID::None;
	return m_layer->getVoxel(x, y);
}

LayerReference& LayerReference::operator =(const LayerReference& other) {
	// Remove old reference.
	if (isValid()) m_layerManager->removeLayerReference(*m_layer);

	// Copy data.
	m_layer = other.m_layer;
	m_layerManager = other.m_layerManager;

	// Incremenet reference counter.
	if (isValid()) m_layerManager->addLayerReference(*m_layer);

	// Return self.
	return *this;
}
