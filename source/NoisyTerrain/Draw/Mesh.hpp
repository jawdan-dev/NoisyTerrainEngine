#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Draw/Shader.hpp>

class Mesh {
private:
	GLuint m_modelBuffer, m_elementBuffer;
	List<Vector3> m_vertices, m_colors;
	List<Vector2> m_uvs;
	List<uint16_t> m_indices;

	uint16_t m_renderCount;
	bool m_verticesEnabled : 1,
		m_colorsEnabled : 1,
		m_uvsEnabled : 1,
		m_indicesEnabled : 1;

public:
	Mesh();
	Mesh(const Mesh& other) = delete;
	~Mesh();

public:
	J_GETTER_DIRECT(getVBO, m_modelBuffer, GLuint);
	J_GETTER_DIRECT(getEBO, m_elementBuffer, GLuint);
	J_GETTER_DIRECT(getStaticSize, (m_verticesEnabled ? sizeof(Vector3) : 0) + (m_colorsEnabled ? sizeof(Vector3) : 0) + (m_uvsEnabled ? sizeof(Vector2) : 0), size_t);
	J_GETTER_DIRECT(getRenderCount, m_renderCount, uint16_t);
	J_GETTER_DIRECT(getVerticesEnabled, m_verticesEnabled, bool);
	J_GETTER_DIRECT(getColorsEnabled, m_colorsEnabled, bool);
	J_GETTER_DIRECT(getUVsEnabled, m_uvsEnabled, bool);
	J_GETTER_DIRECT(getIndicesEnabled, m_indicesEnabled, bool);

public:
	void load(const char* file);
	void clear();

public:
	J_SETTER_DIRECT(setVertices, const List<Vector3>& vertices, m_vertices = vertices);
	J_SETTER_DIRECT(setColors, const List<Vector3>& colors, m_colors = colors);
	J_SETTER_DIRECT(setUVs, const List<Vector2>& uvs, m_uvs = uvs);
	J_SETTER_DIRECT(setIndices, const List<uint16_t>& indices, m_indices = indices);
	void commit();
};