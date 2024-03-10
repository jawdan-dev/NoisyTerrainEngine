#pragma once
#include <Engine/NoisyTerrain.hpp>

#include <Engine/GameObjects/GameObject.hpp>
#include <GameObjects/Entities/EntityBody.hpp>

class EntityObjectBase : public GameObject {
private:
	// Config.
	static constexpr const float s_gravityAcceleration = 9.81f * 2.0f;
	static constexpr const float s_moveFactor = 0.01f;

private:
	// TODO: Health.
	// TODO: Attacks.
	// TODO: Inventory(?).
	// TODO: (De)buffs
	// Knockback / duration

private:
	ColliderCube m_environmentCollider;
	Vector3 m_velocity, m_safeVelocity;
	float m_gravityMultiplier;
	bool m_onGround, m_onWall;

private:
	EntityBody* m_entityBody;

public:
	EntityObjectBase(const ColliderCube& environmentCollider, EntityBody* const entityBody = nullptr);
	EntityObjectBase(const EntityObjectBase& other) = delete;
	virtual ~EntityObjectBase();

public:
	J_GETTER_DIRECT(getVelocity, m_velocity, Vector3&);
	J_GETTER_DIRECT_MUT(getVelocity, m_velocity, Vector3&);
	J_GETTER_DIRECT(getSafeVelocity, m_safeVelocity, Vector3&);
	J_GETTER_DIRECT(getGravityMultiplier, m_gravityMultiplier, float);
	J_GETTER_DIRECT_MUT(getGravityMultiplier, m_gravityMultiplier, float&);
	J_GETTER_DIRECT(isOnGround, m_onGround, bool);
	J_GETTER_DIRECT(isOnWall, m_onWall, bool);

public:
	void onProcess() override;

public:
	void onDraw() override;
	void drawColliders() const;
};