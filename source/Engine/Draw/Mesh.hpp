#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Draw/StaticData.hpp>

class ModelMesh {
private:
	Mutex m_mutex;

private:
	GLuint m_VBO, m_EBO;
	size_t m_VBOUploadStart, m_VBOUploadRemaining,
		m_EBOUploadStart, m_EBOUploadRemaining;
	size_t m_renderCount, m_activeStride;

private:
	bool m_updated;
	Map<String, size_t> m_activeOffsets;
	Map<String, StaticData> m_data;
	List<uint32_t> m_indices;

public:
	ModelMesh();
	ModelMesh(const ModelMesh& other) = delete;
	~ModelMesh();

public:
	J_GETTER_DIRECT(getVBO, m_VBO, GLuint);
	J_GETTER_DIRECT(getEBO, m_EBO, GLuint);
	J_GETTER_DIRECT(getRenderCount, m_renderCount, size_t);
	J_GETTER_DIRECT(getRemainingUpload, m_VBOUploadRemaining + m_EBOUploadRemaining, size_t);
	J_GETTER_DIRECT(getStaticStride, m_activeStride, size_t);
	J_GETTER_DIRECT(getIndicesEnabled, m_EBOUploadStart + m_EBOUploadRemaining > 0, bool);
	const size_t getActiveOffset(const String& staticName) const;

public:
	template <typename T>
	void set(const String& staticName, const List<T>& data);
	void set(const String& staticName, const void* const data, const size_t dataLength, const size_t dataStride);
	void setIndices(const List<uint32_t>& indices);

public:
	void clear();
	void upload(const size_t uploadMax = SIZE_MAX);

public:
	void lock();
	void unlock();
};

#include "Mesh.ipp"