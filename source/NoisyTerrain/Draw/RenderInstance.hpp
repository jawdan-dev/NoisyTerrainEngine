#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Draw/InstanceData.hpp>
#include <NoisyTerrain/Draw/Mesh.hpp>

class RenderInstance {
private:
	size_t m_instanceCount, m_staticInstanceCount;
	void* m_instanceData;
	size_t m_instanceDataCount;
	bool m_instancesUpdated;

	GLuint m_vao, m_ivbo;
	Shader* m_shader;
	Mesh* m_mesh;

public:
	RenderInstance(Shader* const m_shader, Mesh* const m_mesh);
	RenderInstance(const RenderInstance& other) = delete;
	~RenderInstance();

public:
	J_GETTER_DIRECT(getInstanceCount, m_instanceCount, size_t);

public:
	void addInstance(InstanceData& instanceData, const bool isStatic = false);

public:
	void draw(const Matrix4& viewProjection);
	void clear();
};