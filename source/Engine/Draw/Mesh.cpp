#include "Mesh.hpp"

ModelMesh::ModelMesh() :
	m_mutex(),
	m_VBO(0), m_EBO(0),
	m_VBOUploadStart(0), m_VBOUploadRemaining(0),
	m_EBOUploadStart(0), m_EBOUploadRemaining(0),
	m_renderCount(0), m_activeStride(0),
	m_updated(false),
	m_activeOffsets(), m_data(), m_indices() {}
ModelMesh::~ModelMesh() {
	clear();
}

const size_t ModelMesh::getActiveOffset(const String& staticName) const {
	// Find offset.
	auto it = m_activeOffsets.find(staticName);
	if (it == m_activeOffsets.end()) return SIZE_MAX;
	// Return offset.
	return it->second;
}

void ModelMesh::set(const String& staticName, const void* const data, const size_t dataLength, const size_t dataStride) {
	// Check and remove old data.
	auto it = m_data.find(staticName);
	if (it != m_data.end()) m_data.erase(it);

	// Insert new data.
	m_data.emplace(staticName, StaticData(data, dataLength, dataStride));

	// Update details.
	m_updated = true;
}
void ModelMesh::setIndices(const List<uint32_t>& indices) {
	// Set data.
	m_indices = indices;

	// Update details.
	m_updated = true;
}

void ModelMesh::clear() {
	// Cleanup data.
	m_activeOffsets.clear();
	m_data.clear();
	m_indices.clear();

	// Clear details.
	m_VBOUploadStart = 0;
	m_VBOUploadRemaining = 0;
	m_EBOUploadStart = 0;
	m_EBOUploadRemaining = 0;
	m_renderCount = 0;
	m_activeStride = 0;
	m_updated = false;

	// Cleanup buffers.
	if (m_VBO) {
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}
	if (m_EBO) {
		glDeleteBuffers(1, &m_EBO);
		m_EBO = 0;
	}
}
void ModelMesh::upload(const size_t uploadMax) {
	// Check for model updates.
	if (m_updated) {
		// Update active information.
		size_t bufferCount = 0;
		m_activeStride = 0;
		m_activeOffsets.clear();
		for (auto it = m_data.begin(); it != m_data.end(); it++) {
			// Add to offset list.
			m_activeOffsets.emplace(it->first, m_activeStride);
			// Update details.
			m_activeStride += it->second.getDataStride();
			bufferCount = Math::max(bufferCount, it->second.getDataLength());
		}

		// Get buffer size.
		const size_t bufferSize = m_activeStride * bufferCount;

		// Create GPU storage.
		if (!m_VBO) glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Update details.
		m_updated = false;
		m_VBOUploadStart = 0;
		m_VBOUploadRemaining = bufferCount;
		m_EBOUploadStart = 0;
		m_EBOUploadRemaining = 0;

		if (m_indices.size() > 0) {
			// Create indices.
			if (!m_EBO) glGenBuffers(1, &m_EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

			// Set indicies.
			const size_t indicesSize = m_indices.size();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indicesSize, nullptr, GL_STATIC_DRAW);

			// Update details.
			m_renderCount = 0;
			m_EBOUploadRemaining = indicesSize;

			// Unbind.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		} else {
			// Update details.
			m_renderCount = bufferCount;
		}
	}

	// Check for valid VBO.
	if (!m_VBO) return;

	// Upload VBO.
	if (m_VBOUploadRemaining > 0) {
		// Bind buffer.
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		// Upload data to GPU.
		const size_t uploadAmount = Math::min(m_VBOUploadRemaining, uploadMax);
		for (auto it = m_data.begin(); it != m_data.end(); it++) {
			// Get data.
			const StaticData& data = it->second;
			const size_t dataSize = data.getDataStride();

			// Get data offset.
			const size_t staticOffset = getActiveOffset(it->first);
			if (staticOffset == SIZE_MAX) break;

			for (size_t i = 0; i < uploadAmount; i++) {
				if (data.getDataLength() < i + m_VBOUploadStart) break;

				// Get upload offset.
				const size_t dataIndex = (i + m_VBOUploadStart);
				const size_t dataOffset = dataIndex * dataSize;
				const size_t uploadOffset = dataIndex * m_activeStride;

				// Upload data.
				glBufferSubData(
					GL_ARRAY_BUFFER,
					uploadOffset + staticOffset,
					dataSize,
					(void*)((uintptr_t)data.getData() + dataOffset)
				);
			}
		}

		// Unbind.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Update details.
		m_VBOUploadRemaining -= uploadAmount;
		m_VBOUploadStart += uploadAmount;
	}

	// Upload EBO.
	if (m_EBOUploadRemaining > 0) {
		// Bind buffer.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		// Upload data to GPU.
		const size_t uploadAmount = Math::min(m_EBOUploadRemaining, uploadMax - (uploadMax % 3));

		// Copy data.
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER,
			m_EBOUploadStart * sizeof(uint32_t),
			uploadAmount * sizeof(uint32_t),
			m_indices.data() + m_EBOUploadStart
		);

		// Unbind.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Update details.
		m_EBOUploadRemaining -= uploadAmount;
		m_EBOUploadStart += uploadAmount;
		m_renderCount = m_EBOUploadStart;
	}
}

void ModelMesh::lock() {
	m_mutex.lock();
}
void ModelMesh::unlock() {
	m_mutex.unlock();
}
