#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Draw/Shader/Shader.hpp>

enum class ColliderType : uint8_t {
	None,
	Cube,
	Sphere,
};

class ColliderBase {
public:
	static Shader m_debugShader;

private:
	ColliderType m_colliderType;

public:
	ColliderBase(const ColliderType type);
	virtual ~ColliderBase();

public:
	J_GETTER_DIRECT(getColliderType, m_colliderType, ColliderType);

public:
	const bool checkOverlapping(const ColliderBase* const other) const;

public:
	virtual void draw(const Vector3& color) const = 0;
};