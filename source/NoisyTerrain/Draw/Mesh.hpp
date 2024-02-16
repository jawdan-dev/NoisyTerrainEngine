#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Draw/StaticData.hpp>

class Mesh {
private:
	Map<String, StaticData> m_data;
	List<uint32_t> m_indices;

private:
	GLuint m_modelBuffer, m_elementBuffer;

	bool m_indicesEnabled;
	size_t m_renderCount, m_activeStaticStride;
	Map<String, size_t> m_activeOffsets;

public:
	Mesh();
	Mesh(const Mesh& other) = delete;
	~Mesh();

public:
	J_GETTER_DIRECT(getVBO, m_modelBuffer, GLuint);
	J_GETTER_DIRECT(getEBO, m_elementBuffer, GLuint);
	J_GETTER_DIRECT(getRenderCount, m_renderCount, size_t);
	J_GETTER_DIRECT(getActiveStaticStride, m_activeStaticStride, size_t);
	J_GETTER_DIRECT(getIndicesEnabled, m_indicesEnabled, bool);
	const size_t getActiveOffset(const String& staticName) const;

public:
	void load(const char* file);
	void clear();

public:
	template <typename T>
	void set(const String& staticName, const List<T>& data);
	void set(const String& staticName, const void* const data, const size_t dataLength, const size_t dataStride);
	J_SETTER_DIRECT(setIndices, const List<uint32_t>& indices, m_indices = indices;);

	void commit();
};

#include "Mesh.ipp"