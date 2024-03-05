#pragma once
#include "InstanceData.hpp"

template <typename T>
inline void InstanceData::setData(const char* attributeName, const T& data) {
	setData(attributeName, &data, sizeof(data));
}