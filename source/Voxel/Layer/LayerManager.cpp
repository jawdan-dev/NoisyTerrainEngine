#include "LayerManager.hpp"

LayerManager::LayerManager() :
	m_layerLock(), m_layers(), m_disabled(false) {}
LayerManager::~LayerManager() {}

const LayerReference LayerManager::createLayerReference(const Layer& layer) {
	if (m_disabled) return LayerReference(nullptr, nullptr);

	// Find layer.
	auto it = m_layers.find(layer);
	if (it == m_layers.end())
		it = m_layers.emplace(layer, 0).first;

	// Update counter.
	it->second++;

	// Return reference.
	return LayerReference(this, &it->first);
}

void LayerManager::addLayerReference(const Layer& layer) {
	if (m_disabled) return;

	// Find layer.
	auto it = m_layers.find(layer);
	if (it == m_layers.end())
		it = m_layers.emplace(layer, 0).first;

	// Add reference.
	it->second++;
}
void LayerManager::removeLayerReference(const Layer& layer) {
	if (m_disabled) return;

	// Find layer.
	auto it = m_layers.find(layer);
	if (it == m_layers.end()) {
		J_WARNING("LayerManager.cpp: Failed to find layer to remove.\n");
		return;
	}

	// Decrement reference.
	it->second--;
	if (it->second > 0) return;

	// Remove reference.
	m_layers.erase(it);
}
void LayerManager::disable() {
	m_disabled = true;
}

void LayerManager::lock() {
	m_layerLock.lock();
}
void LayerManager::unlock() {
	m_layerLock.unlock();
}