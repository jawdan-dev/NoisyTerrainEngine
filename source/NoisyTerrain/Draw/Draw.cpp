#include "Draw.hpp"

J_SINGLETON_DEF(DrawManager);

DrawManager::DrawManager() :
	m_projection(), m_view(),
	m_viewProjection(),
	m_renderInstances() {}
DrawManager::~DrawManager() {}

void DrawManager::updateViewProjection() {
	// MVP -> (P * V) * M.
	m_viewProjection = m_projection * m_view;
}

void DrawManager::clear() {
	// Clear buffer.
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset render instance count.
	for (auto it = m_renderInstances.begin(); it != m_renderInstances.end();) {
		if (it->second->getInstanceCount() <= 0) {
			// Remove render instance.
			delete it->second;
			it = m_renderInstances.erase(it);
			printf("Removed render instance.\n");
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
}

void DrawManager::draw(Mesh& mesh, InstanceData& instanceData, const bool isStatic) {
	if (this == nullptr) return;

	// Find render instance.
	const RenderKey key(instanceData.getShader(), &mesh);
	auto it = m_renderInstances.find(key);
	if (it == m_renderInstances.end()) {
		// Create render instance.
		it = m_renderInstances.emplace(key, new RenderInstance(key.first, key.second)).first;
		printf("Created render instance.\n");
	}

	// Insert instance.
	it->second->addInstance(instanceData, isStatic);
}
