#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Draw/Shader.hpp>

class InstanceData {
private:
	Shader* m_shader;
	void* m_data;

public:
	InstanceData(Shader* const shader);
	InstanceData(const InstanceData& other) = delete;
	~InstanceData();

public:
	J_GETTER_DIRECT_MUT(getData, m_data, void* const);
	J_GETTER_DIRECT_MUT(getShader, m_shader, Shader* const);

public:
	template <typename T>
	void setData(const char* attributeName, const T& data);
	void setData(const char* attributeName, const void* const data, const size_t dataSize);
};

#include "InstanceData.ipp"