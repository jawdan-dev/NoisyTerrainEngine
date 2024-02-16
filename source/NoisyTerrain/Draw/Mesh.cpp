#include "Mesh.hpp"

Mesh::Mesh() :
	m_data(),
	m_indices(),
	m_modelBuffer(0), m_elementBuffer(0),
	m_indicesEnabled(false),
	m_renderCount(0),
	m_activeStaticStride(0), m_activeOffsets() {}
Mesh::~Mesh() {
	// Cleanup.
	if (m_modelBuffer) glDeleteBuffers(1, &m_modelBuffer);
	if (m_elementBuffer) glDeleteBuffers(1, &m_elementBuffer);
}

const size_t Mesh::getActiveOffset(const String& staticName) const {
	// Find offset.
	auto it = m_activeOffsets.find(staticName);
	if (it == m_activeOffsets.end()) return SIZE_MAX;
	// Return offset.
	return it->second;
}

void Mesh::load(const char* file) {
	// TODO:
}
void Mesh::clear() {
	// Clear data.
	m_data.clear();
	m_indices.clear();
}

void Mesh::set(
	const String& staticName, const void* const data,
	const size_t dataLength, const size_t dataStride
) {
	// Check and remove old data.
	auto it = m_data.find(staticName);
	if (it != m_data.end()) m_data.erase(it);

	// Insert new data.
	m_data.emplace(staticName, StaticData(data, dataLength, dataStride));
}
void Mesh::commit() {
	// Get enabled vertex information.
	m_indicesEnabled = m_indices.size() > 0;

	// Update active information.
	m_activeStaticStride = 0;
	m_activeOffsets.clear();
	size_t bufferCount = 0;
	for (auto it = m_data.begin(); it != m_data.end(); it++) {
		// Add to offset list.
		m_activeOffsets.emplace(it->first, m_activeStaticStride);
		// Update details.
		m_activeStaticStride += it->second.getDataStride();
		bufferCount = __max(bufferCount, it->second.getDataLength());
	}

	// Get buffer size.
	const size_t bufferSize = m_activeStaticStride * bufferCount;

	// Create GPU storage.
	if (m_modelBuffer == 0) glGenBuffers(1, &m_modelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	// Upload data to GPU.
	for (auto it = m_data.begin(); it != m_data.end(); it++) {
		// Get data.
		const StaticData& data = it->second;
		// Get offset.
		const size_t dataOffset = getActiveOffset(it->first);
		if (dataOffset == SIZE_MAX) continue;

		// Copy data.
		for (size_t i = 0; i < data.getDataLength(); i++) {
			glBufferSubData(
				GL_ARRAY_BUFFER,
				(i * m_activeStaticStride) + dataOffset,
				data.getDataStride(),
				data.getData() + (i * data.getDataStride())
			);
		}
	}

	// Unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (m_indicesEnabled) {
		// Create indices.
		if (m_elementBuffer == 0) glGenBuffers(1, &m_elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);

		// Set indicies.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

		// Update details.
		m_renderCount = m_indices.size();

		// Unbind.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		// Update details.
		m_renderCount = bufferCount;
	}
}