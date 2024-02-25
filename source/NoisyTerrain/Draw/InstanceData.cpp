#include "InstanceData.hpp"

InstanceData::InstanceData(Shader* const shader) :
	m_shader(shader), m_data(nullptr) {
	// Create data.
	const size_t allocSize = shader->getTotalInstanceSize();
	if (allocSize > 0) m_data = calloc(allocSize, 1);
}
InstanceData::~InstanceData() {
	// Cleanup data.
	if (m_data) {
		free(m_data);
		m_data = nullptr;
	}
}

void InstanceData::setData(const char* attributeName, const void* const data, const size_t dataSize) {
	// Get attribute.
	const ShaderAttribute* const attributeInformation = m_shader->getAttribute(attributeName);
	if (attributeInformation == nullptr) return;

	// Verify data.
	if (m_data == nullptr ||
		attributeInformation->m_isStatic ||
		attributeInformation->m_dataSize != dataSize) return;

	// Set data.
	memcpy(
		(void*)((uintptr_t)m_data + attributeInformation->m_dataOffset),
		data,
		dataSize
	);
}
