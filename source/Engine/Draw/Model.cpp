#include "Model.hpp"

Model::Model() :
	m_mutex(),
	m_activeMesh(s_maxBuffers - 1) {}
Model::~Model() {}

void Model::load(const char* filePath) {
	// Macro for switching a 4 character file extension.
#	define EXTENSION(str) ( \
		(uint64_t)str[0] << 0 | (uint64_t)str[1] << 16 |  \
		(uint64_t)str[2] << 32 | (uint64_t)str[3] << 48)

	// Get file extension.
	if (strlen(filePath) <= 4) J_ERROR_EXIT("Model.cpp: File path is too short.\n");
	const String filePathStr = String("Assets/") + filePath;
	const String extensionString = filePathStr.substr(filePathStr.size() - 4);
	const uint64_t extension = EXTENSION(extensionString.c_str());

	// Open file.
	FILE* const file = fopen(filePathStr.c_str(), "rb");
	if (file == nullptr) J_ERROR_EXIT("Model.cpp: Failed to open %s\n", filePath);

	// Get mesh to update.
	ModelMesh& mesh = getInactiveMesh();
	mesh.lock();
	mesh.clear();

	// Load.
	switch (extension) {
		default: J_WARNING("Model.cpp: File extension '%s' not supported\n", extensionString.c_str()); break;
		case EXTENSION(".obj"): loadOBJ(file, mesh); break;
	}

	// Error check.
	if (!mesh.hasUpdated()) J_ERROR("Model.cpp: Failed to load model %s.\n", filePath);

	// Cleanup.
	mesh.unlock();
	fclose(file);
#	undef EXTENSION
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