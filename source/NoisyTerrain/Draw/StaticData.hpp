#pragma once
#include <NoisyTerrain/Core/Core.hpp>

class StaticData {
private:
	size_t m_length, m_stride;
	void* m_data;

public:
	StaticData(const void* const data, const size_t dataLength, const size_t dataStride);
	StaticData(const StaticData& other);
	~StaticData();

public:
	J_GETTER_DIRECT(getDataStride, m_stride, size_t);
	J_GETTER_DIRECT(getDataLength, m_length, size_t);
	J_GETTER_DIRECT(getDataSize, m_stride* m_length, size_t);
	J_GETTER_DIRECT(getData, m_data, void* const);
};