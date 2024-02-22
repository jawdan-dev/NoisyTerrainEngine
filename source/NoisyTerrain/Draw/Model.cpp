#include "Model.hpp"

Model::Model() :
	m_mutex(),
	m_activeMesh(s_maxBuffers - 1) {}
Model::~Model() {}

void Model::load(const char* file) {
	// TODO:
	J_ERROR_EXIT("Model.cpp: Model loading not yet implemented.\n");
}
const bool Model::upload() {
	// Get mesh.
	ModelMesh& mesh = getInactiveMesh();

	// Upload mesh.
	mesh.lock();
	mesh.upload();

	// Check if meshes can be swapped.
	if (mesh.getRenderCount() > 0 && mesh.getRemainingUpload() <= 0) {
		// Update active mesh.
		m_activeMesh = getInactiveIndex();
		// Unlock.
		mesh.unlock();

		// Get clear old mesh.
		if (getInactiveIndex() != m_activeMesh) {
			ModelMesh& inactiveMesh = getInactiveMesh();
			inactiveMesh.lock();
			inactiveMesh.clear();
			inactiveMesh.unlock();
		}

		// Model finished uploading.
		return true;
	}

	// Unlock.
	mesh.unlock();

	// Model still uploading.
	return false;
}

const bool Model::tryLock() {
	return m_mutex.try_lock();
}
void Model::lock() {
	m_mutex.lock();
}
void Model::unlock() {
	m_mutex.unlock();
}