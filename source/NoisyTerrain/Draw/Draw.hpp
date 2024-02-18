#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <NoisyTerrain/Draw/RenderInstance.hpp>

class DrawManager {
	J_SINGLETON(DrawManager)
#	define Draw J_SINGLETON_GET(DrawManager)

private:
	Matrix4 m_projection, m_view, m_viewProjection;
	typedef std::pair<Shader*, Model*> RenderKey;
	Map<RenderKey, RenderInstance*> m_renderInstances;

public:
	DrawManager();
	DrawManager(const DrawManager& other) = delete;
	~DrawManager();

public:
	J_SETTER_DIRECT(setProjection, const Matrix4& projection, m_projection = projection; updateViewProjection());
	J_SETTER_DIRECT(setView, const Matrix4& view, m_view = view; updateViewProjection());

private:
	void updateViewProjection();

public:
	void clear();
	void drawAll();

public:
	void draw(Model& model, InstanceData& instanceData, const void* const staticID = nullptr);
	void undrawStatic(const void* const staticID);
};
