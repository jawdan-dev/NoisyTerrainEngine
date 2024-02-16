#include "StaticData.hpp"

StaticData::StaticData(const void* const data, const size_t dataLength, const size_t dataStride) :
	m_length(dataLength), m_stride(dataStride),
	m_data(nullptr) {
	// Check if data length valid.
	const size_t dataSize = m_length * m_stride;
	if (dataSize > 0) {
		// Create data.
		m_data = malloc(dataSize);
		// Copy data.
		memcpy(m_data, data, dataSize);
	}
}
StaticData::StaticData(const StaticData& other) :
	StaticData(other.m_data, other.m_length, other.m_stride) {}
StaticData::~StaticData() {
	// Cleanup.
	if (m_data != nullptr) {
		free(m_data);
		m_data = nullptr;
	}
}