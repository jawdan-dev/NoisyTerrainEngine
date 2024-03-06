#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Draw/Rendering/InstanceData.hpp>
#include <Engine/Draw/Model/Model.hpp>

class RenderInstance {
private:
	size_t m_instanceCount;
	List<const void*> m_staticInstances;

	void* m_instanceData;
	size_t m_instanceDataCount;
	bool m_instancesUpdated;

private:
	uint16_t m_drawSkip;
	GLuint m_vao, m_ivbo;

	Shader* m_shader;

private:
	bool m_modelUpdated;
	GLuint m_lastUsedVBO;
	Model* m_model;

public:
	RenderInstance(Shader* const shader, Model* const model);
	RenderInstance(const RenderInstance& other) = delete;
	~RenderInstance();

public:
	J_GETTER_DIRECT(getInstanceCount, m_instanceCount, size_t);

public:
	void addInstance(InstanceData& instanceData, const void* const staticID = nullptr);
	void removeStaticInstance(const void* const staticID);

private:
	void updateInstances();
	void updateVAO(ModelMesh& mesh);

public:
	void draw(const Matrix4& viewProjection);
	void clear();

private:
	const bool resize(const size_t dataCount);
	const bool insert(const size_t index, void* const data);
	const bool remove(const size_t index);
};