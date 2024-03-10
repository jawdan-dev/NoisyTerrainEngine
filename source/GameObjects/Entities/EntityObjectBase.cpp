#include "EntityObjectBase.hpp"

#include <GameObjects/VoxelManagerObject.hpp>

EntityObjectBase::EntityObjectBase(const ColliderCube& environmentCollider, EntityBody* const entityBody) :
	m_environmentCollider(environmentCollider),
	m_velocity(0.0f, 0.0f, 0.0f), m_safeVelocity(0.0f, 0.0f, 0.0f), m_gravityMultiplier(1.0f),
	m_onGround(false), m_onWall(false),
	m_entityBody(entityBody) {}
EntityObjectBase::~EntityObjectBase() {}

void EntityObjectBase::onProcess() {
	// Get total velocity with respect to gravity.
	const float halfGravityFactor = s_gravityAcceleration * m_gravityMultiplier * Time.delta() * 0.5f;
	m_velocity.y() -= halfGravityFactor;
	const Vector3 totalVelocity = m_velocity * Time.delta();
	m_velocity.y() -= halfGravityFactor;

	// No collision needed if not moving.
	if (totalVelocity.lengthSquared() <= 0) return;

	// Draw player.
	const ColliderCube playerCollider(getPosition() + m_environmentCollider.getPosition(), m_environmentCollider.getSize());

	// Get voxel entity.
	VoxelManagerObject* const voxelManagerObject =
		Window.getGameObjectManager()->getFirstGameObjectOfType<VoxelManagerObject>();
	if (voxelManagerObject == nullptr) return;

	// Get chunk manager.
	ChunkManager* const chunkManager = voxelManagerObject->getChunkManager();
	if (chunkManager == nullptr) return;

	// Make sure the player chunk is initialized.
	chunkManager->lockChunks();
	Chunk* const playerChunk = chunkManager->getChunk(VoxelLocation(getPosition().x(), 0, getPosition().z()));
	if (playerChunk == nullptr) {
		chunkManager->unlockChunks();
		return;
	}
	const bool playerChunkInitialized = playerChunk->isInitialized();
	chunkManager->unlockChunks();
	if (!playerChunkInitialized) return;

	// Get velocity bounds.
	const Vector3 velocityStart = playerCollider.getPosition();
	const Vector3 velocityEnd = velocityStart + totalVelocity;
	const Vector3 velocityBoundMin(
		Math::floor(Math::min(velocityStart.x(), velocityEnd.x())),
		Math::floor(Math::min(velocityStart.y(), velocityEnd.y())),
		Math::floor(Math::min(velocityStart.z(), velocityEnd.z()))
	), velocityBoundMax(
		Math::ceil(Math::max(velocityStart.x(), velocityEnd.x()) + playerCollider.getSize().x()),
		Math::ceil(Math::max(velocityStart.y(), velocityEnd.y()) + playerCollider.getSize().y()),
		Math::ceil(Math::max(velocityStart.z(), velocityEnd.z()) + playerCollider.getSize().z())
	);

	// Get voxel colliders.
	List<ColliderCube> environmentColliders;

	// Convert location.
	const VoxelLocation
		voxelMin(velocityBoundMin.x(), velocityBoundMin.y(), velocityBoundMin.z()),
		voxelMax(velocityBoundMax.x(), velocityBoundMax.y(), velocityBoundMax.z());

	// Check voxels in bounds.
	bool canPlace;
	VoxelLocation location;
	chunkManager->lockChunks();
	for (location.x() = voxelMin.x(); location.x() < voxelMax.x(); location.x()++) {
		for (location.y() = voxelMin.y(); location.y() <= voxelMax.y(); location.y()++) {
			for (location.z() = voxelMin.z(); location.z() < voxelMax.z(); location.z()++) {
				// Get chunk.
				Chunk* const chunk = chunkManager->getChunk(location);

				if (chunk != nullptr && chunk->isInitialized()) {
					// Get voxel.
					chunk->lockPlacement();
					canPlace = chunk->getVoxel(location.getRelativeLocation()) != VoxelID::None;
					chunk->unlockPlacement();
				} else {
					// Chunk invalid. Place wall.
					canPlace = true;
				}

				// Insert collider.
				if (canPlace)
					environmentColliders.emplace_back(Vector3(location.x(), location.y(), location.z()), Vector3(1.0f, 1.0f, 1.0f));
			}
		}
	}
	chunkManager->unlockChunks();

	// Check for collisions.
	m_safeVelocity = Vector3(0, 0, 0);
	static const bool checkCanStep[3] { true, false, true, };
	size_t checks[3] { 0, 2, 1, };
	// Prioritize largest horizontal velocity.
	if (totalVelocity.z() > totalVelocity.x()) {
		checks[0] = 2;
		checks[1] = 0;
	}

	bool collided[3] { false, false, false, };
	bool hasStepped = false;
	for (size_t i = 0; i < 3; i++) {
		// Make sure we're working on a in-use axis.
		const size_t& targetCheck = checks[i];
		if (totalVelocity[targetCheck] == 0.0f) continue;

		// Step velocity.
		const float sign = Math::sign(totalVelocity[targetCheck]);
		for (float totalFactor = Math::abs(totalVelocity[targetCheck]); totalFactor > 0.0f; totalFactor -= s_moveFactor) {
			// Get factor.
			const float factor = Math::min(totalFactor, s_moveFactor);
			const float moveAmount = factor * sign;

			// Get check values.
			Vector3 checkVelocity(m_safeVelocity);
			checkVelocity[targetCheck] += moveAmount;
			ColliderCube checkCollider(playerCollider.getPosition() + checkVelocity, playerCollider.getSize());

			// Check.
			for (const ColliderCube& collider : environmentColliders)
				if (collided[targetCheck] = collider.checkOverlapping(&checkCollider))
					break;

			// Handle collisions.
			if (collided[targetCheck]) {
				if (!checkCanStep[targetCheck] || hasStepped) break;

				// Check for steps.
				checkVelocity.y() += 1.01f - Math::mod(playerCollider.getPosition().y(), 1.0f);
				checkCollider.setPosition(playerCollider.getPosition() + checkVelocity);
				for (const ColliderCube& collider : environmentColliders)
					if (collided[targetCheck] = collider.checkOverlapping(&checkCollider))
						break;

				// Check for step collision.
				if (collided[targetCheck]) break;

				// Step.
				hasStepped = true;
				m_safeVelocity.y() = checkVelocity.y();
			}

			// Update velocity.
			m_safeVelocity[targetCheck] += moveAmount;
		}
	}

	// Handle player movement.
	setPosition(getPosition() + m_safeVelocity);
	// Reset safe velocity to ignore delta.
	m_safeVelocity /= Time.delta();

	// Handle on surface states.
	m_onGround = collided[1];
	m_onWall = collided[0] || collided[2];

	// Handle gravity.
	if (m_onGround) m_velocity.y() = 0;
}

void EntityObjectBase::onDraw() {
	drawColliders();
	if (m_entityBody != nullptr) m_entityBody->draw(getPosition());
}
void EntityObjectBase::drawColliders() const {
	// Draw colliders.
	const ColliderCube playerCollider(getPosition() + m_environmentCollider.getPosition(), m_environmentCollider.getSize());
	playerCollider.draw(Vector3(1.0f, 0.0f, 0.54f));
}