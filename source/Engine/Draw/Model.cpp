#include "Model.hpp"

Model::Model() :
	m_mutex(),
	m_activeMesh(s_maxBuffers - 1) {}
Model::~Model() {}

void Model::load(const char* file) {
	// TODO:
	J_ERROR_EXIT("Model.cpp: Model loading not yet implemented.\n");
}
const bool Model::upload(const size_t uploadMax) {
	// Get mesh.
	ModelMesh& mesh = getInactiveMesh();

	// Upload mesh.
	mesh.lock();
	mesh.upload(uploadMax);

	// Get remaining upload.
	const size_t remainingUpload = mesh.getRemainingUpload();

	// Check if meshes can be swapped.
	if (mesh.getRenderCount() > 0 && remainingUpload <= 0) {
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
	} else {
		// Unlock.
		mesh.unlock();
	}

	// Return if model still uploading.
	return remainingUpload > 0;
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