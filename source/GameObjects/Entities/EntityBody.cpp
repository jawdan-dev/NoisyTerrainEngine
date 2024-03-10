#include "EntityBody.hpp"

Shader EntityBody::s_entityShader = Shader("Entities/EntityShader.glsl");

EntityBody::EntityBody(const char* filePrefix) :
	m_loaded(false), m_filePrefix(filePrefix),
	m_model_body() {}
EntityBody::~EntityBody() {}

void EntityBody::loadModel(Model& model, const char* suffix) {
	const String filePrefixStr = m_filePrefix;
	model.lock();
	model.load(("Entities/" + filePrefixStr + suffix + ".obj").c_str());
	model.upload();
	model.unlock();
}

void EntityBody::load() {
	if (m_loaded) return;

	// Load.
	loadModel(m_model_body, "Body");

	// Update details.
	m_loaded = true;
}
void EntityBody::draw(const Vector3& position) {
	// Make sure loaded.
	load();

	// Draw.
	InstanceData instanceData(&s_entityShader);
	instanceData.setData("i_offset", position + Vector3(0.0f, 0.6f, 0.0f));
	instanceData.setData("i_scale", 0.25f);
	Draw.draw(m_model_body, instanceData);
}
