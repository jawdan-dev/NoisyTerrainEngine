#include "RenderInstance.hpp"

RenderInstance::RenderInstance(Shader* const shader, Model* const model) :
	m_instanceCount(0), m_staticInstanceCount(0),
	m_instanceData(nullptr),
	m_instanceDataCount(0), m_instancesUpdated(true),
	m_drawSkip(0),
	m_vao(0), m_ivbo(0),
	m_shader(shader),
	m_modelUpdated(false), m_model(model) {}
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

void RenderInstance::updateInstances() {
	if (m_instanceCount <= 0) return;

	if (!m_ivbo) {
		// Initialize instance data.
		glGenBuffers(1, &m_ivbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
		glBufferData(GL_ARRAY_BUFFER, m_instanceDataCount * m_shader->getTotalInstanceSize(), m_instanceData, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else if (m_instancesUpdated) {
		// Update instance data.
		glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
		// TODO: Update subdata.
		glBufferData(GL_ARRAY_BUFFER, m_instanceDataCount * m_shader->getTotalInstanceSize(), m_instanceData, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
void RenderInstance::updateVAO(ModelMesh& mesh) {
	// Make sure we have the buffers necessary.
	if (!m_ivbo || !mesh.getVBO()) {
		// Destroy VAO.
		if (m_vao) glDeleteVertexArrays(1, &m_vao);
		return;
	}

	// Check if VAO needs to be updated.
	const bool generateVAO = !m_vao;
	if (mesh.getVBO() != m_lastUsedVBO) m_modelUpdated = true;
	if (!generateVAO && !m_modelUpdated) return;

	// Create VAO.
	if (generateVAO) glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Set attributes.
	const List<ShaderAttribute>& attributes = m_shader->getAttributes();
	for (size_t i = 0; i < attributes.size(); i++) {
		// Get attribute.
		const ShaderAttribute& attribute = attributes[i];

		// Ignore non-used static.
		size_t offset = (attribute.m_isStatic ? 0 : attribute.m_dataOffset);
		if (attribute.m_isStatic) {
			// Get static offset.
			const size_t staticOffset = mesh.getActiveOffset(attribute.m_name);
			if (staticOffset == -1) continue;
			offset = staticOffset;
		}

		// Bind buffer.
		if (attribute.m_isStatic) glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
		else glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);

		// Get attribute information.
		const uint16_t elementCount = glGetTypeElementCount(attribute.m_glType);
		const GLenum type = glGetTypeBase(attribute.m_glType);
		const size_t stride = attribute.m_isStatic ? mesh.getStaticStride() : m_shader->getTotalInstanceSize();
		const uint8_t divisor = attribute.m_isStatic ? 0 : 1;

		// Set attribute information.
		switch (type) {
			default: glVertexAttribPointer(attribute.m_location, elementCount, type, GL_FALSE, stride, (void*)offset); break;

			case GL_UNSIGNED_INT:
			case GL_INT: glVertexAttribIPointer(attribute.m_location, elementCount, type, stride, (void*)offset); break;

			case GL_DOUBLE: glVertexAttribLPointer(attribute.m_location, elementCount, type, stride, (void*)offset); break;
		}
		glVertexAttribDivisor(attribute.m_location, divisor);
		glEnableVertexAttribArray(attribute.m_location);
	}

	// Unbind VAO.
	glBindVertexArray(0);

	// Update details.
	m_modelUpdated = false;
	m_lastUsedVBO = mesh.getVBO();
	m_drawSkip = 2;
}

void RenderInstance::draw(const Matrix4& viewProjection) {
	if (this == nullptr || m_instanceCount <= 0) return;

	// Get active mesh.
	m_model->lock();
	ModelMesh& mesh = m_model->getActiveMesh();
	mesh.lock();

	// Check if mesh ready to be used.
	if (mesh.getRenderCount() <= 0 ||
		!mesh.getVBO() &&
		mesh.getRemainingUpload() > 0) {
		// Make sure rebuild happens next render.
		m_modelUpdated = true;
		// Unlock access.
		mesh.unlock();
		m_model->unlock();
		return;
	}

	// Update data.
	updateInstances();

	// Update VAO.
	updateVAO(mesh);

	// Draw.
	if (!m_drawSkip <= 0 && m_vao && m_ivbo && m_shader->getProgram() && mesh.getVBO()) {
		// Bind.
		glUseProgram(m_shader->getProgram());
		glBindVertexArray(m_vao);

		// Set uniforms.
		const GLint viewProjectionLocation = glGetUniformLocation(m_shader->getProgram(), "u_viewProjection");
		if (viewProjectionLocation != -1) glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, viewProjection.getData());

		// Draw.
		if (mesh.getIndicesEnabled()) {
			// Draw indices instanced.
			if (mesh.getEBO()) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getEBO());
				glDrawElementsInstanced(
					GL_TRIANGLES,
					mesh.getRenderCount(), GL_UNSIGNED_INT, nullptr,
					m_instanceCount
				);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		} else {
			// Draw arrays instanced.
			glDrawArraysInstanced(
				GL_TRIANGLES,
				0, mesh.getRenderCount(),
				m_instanceCount
			);
		}

		// Reset bindings.
		glBindVertexArray(0);
		glUseProgram(0);
	}

	// Free locks.
	mesh.unlock();
	m_model->unlock();
}
void RenderInstance::clear() {
	// Clear instances.
	m_instanceCount = m_staticInstanceCount;
}