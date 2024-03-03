#include "Draw.hpp"

J_SINGLETON_DEF(DrawManager);

DrawManager::DrawManager() :
	m_projection(), m_view(),
	m_viewProjection(),
	m_renderInstances() {}
DrawManager::~DrawManager() {
	// Cleanup.
	for (auto it = m_renderInstances.begin(); it != m_renderInstances.end(); it++)
		delete it->second;
	m_renderInstances.clear();
}

void DrawManager::updateViewProjection() {
	// MVP -> (P * V) * M.
	m_viewProjection = m_projection * m_view;
}

void DrawManager::clear() {
	// Clear buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset render instance count.
	for (auto it = m_renderInstances.begin(); it != m_renderInstances.end();) {
		if (it->second->getInstanceCount() <= 0) {
			// Remove render instance.
			delete it->second;
			it = m_renderInstances.erase(it);
			continue;
		}

		// Clear instances.
		it->second->clear();

		// Increment iterator.
		it++;
	}
}
void DrawManager::drawAll() {
	// Draw all render instances.
	for (auto it = m_renderInstances.begin(); it != m_renderInstances.end(); it++) {
		it->second->draw(m_viewProjection);
	}

	// Reset bindings.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void DrawManager::draw(Model& model, InstanceData& instanceData, const void* const staticID) {
	// Find render instance.
	const RenderKey key(instanceData.getShader(), &model);
	auto it = m_renderInstances.find(key);
	if (it == m_renderInstances.end()) {
		// Create render instance.
		it = m_renderInstances.emplace(key, new RenderInstance(key.first, key.second)).first;
	}

	// Insert instance.
	it->second->addInstance(instanceData, staticID);
}
void DrawManager::undrawStatic(const void* const staticID) {
	if (staticID == nullptr) return;

	// Remove static instance from all buffers.
	for (auto it = m_renderInstances.begin(); it != m_renderInstances.end(); it++) {
		it->second->removeStaticInstance(staticID);
	}
}
