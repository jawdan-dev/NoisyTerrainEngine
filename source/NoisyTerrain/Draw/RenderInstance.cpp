#include "RenderInstance.hpp"

RenderInstance::RenderInstance(Shader* const shader, Mesh* const mesh) :
	m_instanceCount(0), m_staticInstanceCount(0),
	m_instanceData(nullptr),
	m_instanceDataCount(0), m_instancesUpdated(true),
	m_vao(0), m_ivbo(0),
	m_shader(shader), m_mesh(mesh) {}
RenderInstance::~RenderInstance() {
	// Cleanup.
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
	if (m_ivbo) glDeleteBuffers(1, &m_ivbo);
	if (m_instanceData != nullptr) free(m_instanceData);
}

const size_t binarySize(const size_t x) {
	size_t a = 1;
	while (a < x) a <<= 1;
	return a;
}
const bool resize(void*& currentData, size_t& currentDataCount, const size_t dataCount, const size_t dataByteSize) {
	if (currentDataCount > dataCount) return true;

	// Get new target size.
	const size_t newSize = binarySize(dataCount) * dataByteSize;

	// Realloc data.
	void* newData = realloc(currentData, newSize);
	if (newData == nullptr) {
		J_WARNING("RenderInstance.cpp: Failed to realloc render instances to %zu bytes\n", newSize);
		return false;
	}

	// Update details.
	currentData = newData;
	currentDataCount = dataCount;
	return true;
}

void RenderInstance::addInstance(InstanceData& instanceData, const bool isStatic) {
	if (this == nullptr ||
		instanceData.getShader() != m_shader ||
		!resize(m_instanceData, m_instanceDataCount, m_instanceCount + 1, m_shader->getTotalInstanceSize())) return;

	// Check if data has been updated.
	const size_t dataLength = m_shader->getTotalInstanceSize();
	void* const targetData = m_instanceData + (m_instanceCount * dataLength);
	const void* const srcData = instanceData.getData();

	if (memcmp(targetData, srcData, dataLength)) {
		// Update data.
		memcpy(targetData, srcData, dataLength);

		// Update details.
		m_instancesUpdated = true;
	}

	// Update details.
	m_instanceCount++;
}

void RenderInstance::draw(const Matrix4& viewProjection) {
	if (this == nullptr || m_instanceCount <= 0 || m_mesh->getRenderCount() <= 0 || m_mesh->getVBO() <= 0) return;

	// Check if instances have been updated.
	if (m_instancesUpdated) {
		if (!m_ivbo) {
			// Initialize instance data.
			glGenBuffers(1, &m_ivbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
			glBufferData(GL_ARRAY_BUFFER, m_instanceDataCount * m_shader->getTotalInstanceSize(), m_instanceData, GL_DYNAMIC_DRAW);
		} else {
			// Update instance data.
			glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
			// TODO: Update subdata.
			glBufferData(GL_ARRAY_BUFFER, m_instanceDataCount * m_shader->getTotalInstanceSize(), m_instanceData, GL_DYNAMIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (!m_vao) {
			// Create VAO.
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			// Set attributes.
			const List<ShaderAttribute> attributes = m_shader->getAttributes();
			for (size_t i = 0; i < attributes.size(); i++) {
				// Get attribute.
				const ShaderAttribute& attribute = attributes[i];

				// Ignore non-used static.
				const void* offset = (void*)(attribute.m_isStatic ? 0 : attribute.m_dataOffset);
				if (attribute.m_isStatic) {
					// Get static offset.
					const size_t staticOffset = m_mesh->getActiveOffset(attribute.m_name);
					if (staticOffset == -1) continue;
					offset = (void*)staticOffset;
				}

				// Bind buffer.
				if (attribute.m_isStatic) glBindBuffer(GL_ARRAY_BUFFER, m_mesh->getVBO());
				else glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);

				// Get attribute information.
				const uint16_t elementCount = glGetTypeElementCount(attribute.m_glType);
				const GLenum type = glGetTypeBase(attribute.m_glType);
				const size_t stride = attribute.m_isStatic ? m_mesh->getActiveStaticStride() : m_shader->getTotalInstanceSize();
				const uint8_t divisor = attribute.m_isStatic ? 0 : 1;

				// Set attribute information.
				glVertexAttribPointer(
					attribute.m_location,
					elementCount, type,
					GL_FALSE,
					stride, offset
				);
				glVertexAttribDivisor(attribute.m_location, divisor);
				glEnableVertexAttribArray(attribute.m_location);
			}

			// Unbind VAO.
			glBindVertexArray(0);
		}

		// Update details.
		m_instancesUpdated = false;
	}

	if (!m_vao || !m_ivbo) return;

	// Unbind VAO.
	glUseProgram(m_shader->getProgram());
	glBindVertexArray(m_vao);

	// Set uniforms.
	const GLint viewProjectionLocation = glGetUniformLocation(m_shader->getProgram(), "u_viewProjection");
	if (viewProjectionLocation != -1) glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, viewProjection.getData());

	// Draw.
	if (m_mesh->getIndicesEnabled() && m_mesh->getEBO()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh->getEBO());
		glDrawElementsInstanced(
			GL_TRIANGLES,
			m_mesh->getRenderCount(), GL_UNSIGNED_INT, nullptr,
			m_instanceCount
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glDrawArraysInstanced(
			GL_TRIANGLES,
			0, m_mesh->getRenderCount(),
			m_instanceCount
		);
	}

	// Restart.
	glBindVertexArray(0);
	glUseProgram(0);
}
void RenderInstance::clear() {
	// Clear instances.
	m_instanceCount = m_staticInstanceCount;
}