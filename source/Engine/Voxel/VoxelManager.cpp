#include "VoxelManager.hpp"

VoxelManager::VoxelManager() : m_layerManager(), m_chunkManager(this) {}
VoxelManager::~VoxelManager() {
	// Cleanup chunk manager threads.
	m_chunkManager.getThreadPool().terminateThreads();

	// Delete references.
	m_layerManager.lock();
	m_layerManager.disable();
	m_layerManager.unlock();
}