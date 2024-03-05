#include "CollisionBase.hpp"

#include <Engine/Collision/ColliderCube.hpp>
#include <Engine/Collision/ColliderSphere.hpp>

Shader ColliderBase::m_debugShader("ColliderDebugShader.glsl");

ColliderBase::ColliderBase(const ColliderType type) : m_colliderType(type) {}
ColliderBase::~ColliderBase() {}

const bool ColliderBase::checkOverlapping(const ColliderBase* const other) const {
	// Lowest type first.
	if (other->getColliderType() < m_colliderType)
		return other->checkOverlapping(this);

	switch (m_colliderType) {
		case ColliderType::Cube: {
			// Get base collider.
			ColliderCube* const baseCollider = (ColliderCube*)this;

			// Handle other collider.
			switch (other->m_colliderType) {
				case ColliderType::Cube: {
					// Cube v Cube.
					ColliderCube* const otherCollider = (ColliderCube*)other;

					// Get bounds.
					const Vector3& baseMin = baseCollider->getPosition();
					const Vector3& otherMin = otherCollider->getPosition();
					const Vector3 baseMax = baseMin + baseCollider->getSize();
					const Vector3 otherMax = otherMin + otherCollider->getSize();

					// Check overlap.
					return
						baseMin.x() < otherMax.x() && baseMax.x() > otherMin.x() &&
						baseMin.y() < otherMax.y() && baseMax.y() > otherMin.y() &&
						baseMin.z() < otherMax.z() && baseMax.z() > otherMin.z();
				} break;

				case ColliderType::Sphere: {
					// Cube v Sphere.
					ColliderSphere* const otherCollider = (ColliderSphere*)other;

					// Get bounds.
					const Vector3& point = otherCollider->getPosition();
					const float radius = otherCollider->getRadius();
					const Vector3& baseMin = baseCollider->getPosition();
					const Vector3 baseMax = baseMin + baseCollider->getSize();

					// Get clamped position.
					const Vector3 clampedPosition = Vector3(
						Math::clamp(point.x(), baseMin.x(), baseMax.x()),
						Math::clamp(point.y(), baseMin.y(), baseMax.y()),
						Math::clamp(point.z(), baseMin.z(), baseMax.z())
					);

					// Check distance.
					return (point - clampedPosition).lengthSquared() < (radius * radius);
				} break;
			}
		} break;
		case ColliderType::Sphere: {
			// Get base collider.
			ColliderSphere* const baseCollider = (ColliderSphere*)this;

			// Handle other collider.
			switch (other->m_colliderType) {
				case ColliderType::Sphere: {
					// Sphere v Sphere.
					ColliderSphere* const otherCollider = (ColliderSphere*)other;

					// Get bounds.
					const float radius = baseCollider->getRadius() + otherCollider->getRadius();

					// Check distance.
					return (baseCollider->getPosition() - otherCollider->getPosition()).lengthSquared() < (radius * radius);
				} break;
			}
		} break;
	}

	// Failed to handle collision.
	J_WARNING("CollisionBase.cpp: Collision did not occur between types '%u' and '%u'", m_colliderType, other->m_colliderType);

	// No collision found.
	return false;
}

