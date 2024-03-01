#include "VoxelManager.hpp"

VoxelManager::VoxelManager() : m_layerManager(), m_chunkManager(this) {}
VoxelManager::~VoxelManager() {
	m_layerManager.lock();
	m_layerManager.disable();
	m_layerManager.unlock();
}