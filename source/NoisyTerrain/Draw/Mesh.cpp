#include "Mesh.hpp"

Mesh::Mesh() :
	m_modelBuffer(0), m_elementBuffer(0),
	m_vertices(), m_colors(), m_uvs(), m_indices(),
	m_renderCount(0),
	m_verticesEnabled(false), m_colorsEnabled(false), m_uvsEnabled(false), m_indicesEnabled(false) {}
Mesh::~Mesh() {
	// Cleanup.
	if (m_modelBuffer) glDeleteBuffers(1, &m_modelBuffer);
	if (m_elementBuffer) glDeleteBuffers(1, &m_elementBuffer);
}

void Mesh::load(const char* file) {

}

void Mesh::commit() {
	// Get enabled vertex information.
	m_verticesEnabled = m_vertices.size() > 0;
	m_colorsEnabled = m_colors.size() > 0;
	m_uvsEnabled = m_uvs.size() > 0;
	m_indicesEnabled = m_indices.size() > 0;

	// Make sure at least the vertices are renderable.
	if (!m_verticesEnabled) {
		m_renderCount = 0;
		return;
	}

	// Get buffer count.
	const size_t bufferCount = m_vertices.size();

	// Get buffer size.
	const size_t bufferSize = getStaticSize() * bufferCount;

	// Create GPU storage.
	if (m_modelBuffer == 0) glGenBuffers(1, &m_modelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	// Upload data to GPU.
	size_t dataOffset = 0;
	for (size_t i = 0; i < bufferCount; i++) {
#		define CopyAttribute(condition, list) \
			if (condition) { \
				if (list.size() > i) \
					glBufferSubData(GL_ARRAY_BUFFER, dataOffset, sizeof(list[i]), &list[i]); \
				dataOffset += sizeof(list[i]); \
			}

		CopyAttribute(m_verticesEnabled, m_vertices);
		CopyAttribute(m_colorsEnabled, m_colors);
		CopyAttribute(m_uvsEnabled, m_uvs);

#		undef CopyAttribute
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