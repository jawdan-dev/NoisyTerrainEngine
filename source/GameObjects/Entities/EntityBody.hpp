#pragma once
#include <Engine/NoisyTerrain.hpp>

class EntityBody {
private:
	static Shader s_entityShader;

private:
	bool m_loaded;
	const char* m_filePrefix;

private:
	Model m_model_body;

public:
	EntityBody(const char* filePrefix);
	EntityBody(const EntityBody& other) = delete;
	~EntityBody();

private:
	void loadModel(Model& model, const char* suffix);

public:
	void load();
	void draw(const Vector3& position);
};
