#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Draw/Mesh.hpp>

class Model {
private:
	Mutex m_mutex;

private:
	static constexpr size_t s_maxBuffers = 2;

	size_t m_activeMesh;
	ModelMesh m_meshes[s_maxBuffers];

public:
	Model();
	Model(const Model& other) = delete;
	~Model();

public:
	J_GETTER_DIRECT(getInactiveIndex, (m_activeMesh + 1) % s_maxBuffers, size_t);
	J_GETTER_DIRECT_MUT(getActiveMesh, m_meshes[m_activeMesh], ModelMesh&);
	J_GETTER_DIRECT_MUT(getInactiveMesh, m_meshes[getInactiveIndex()], ModelMesh&);

public:
	void load(const char* filePath);
	const bool upload(const size_t uploadMax = SIZE_MAX);

public:
	const bool tryLock();
	void lock();
	void unlock();

private:
	void loadOBJ(FILE* const file, ModelMesh& mesh);
};