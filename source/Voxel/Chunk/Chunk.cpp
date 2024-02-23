#include "Chunk.hpp"

#include <Voxel/Chunk/ChunkManager.hpp>
#include <Voxel/VoxelManager.hpp>

#include <NoisyTerrain/Draw/Draw.hpp>

Chunk::PlacementInformation::PlacementInformation(const VoxelID voxelID, const bool force) :
	m_voxelID(voxelID), m_force(force) {}

Chunk::Chunk(ChunkManager* const chunkManager, const ChunkLocation& location) :
	m_chunkManager(chunkManager),
	m_initialized(false), m_drawn(false),
	m_location(location),
	m_layers(new LayerReference[voxelChunkSizeY]()),
	m_model(),
	m_placementMutex(), m_placementQueue() {}
Chunk::~Chunk() {
	// Undraw.
	forceUndraw();

	// Cleanup storage.
	delete[] m_layers;
}

const VoxelID Chunk::getVoxel(const VoxelLocation& location) {
	// Make sure within chunk height.
	if (location.y() < 0 || location.y() >= voxelChunkSizeY)
		return VoxelID::None;

	// Check if in another chunk.
	if (location.x() < 0 || location.z() < 0 ||
		location.x() >= voxelChunkSizeX || location.z() >= voxelChunkSizeZ) {
		// Get chunk voxel.
		m_chunkManager->lockChunks();
		const VoxelID voxelID = m_chunkManager->getVoxel((VoxelLocation)m_location + location);
		m_chunkManager->unlockChunks();

		// Return voxel.
		return voxelID;
	}

	// Get voxel.
	return m_layers[location.y()].getVoxel(location.x(), location.z());
}

void Chunk::queueInitialization() const {
	// Queue initialization.
	m_chunkManager->lockInitialization();
	m_chunkManager->queueInitialization(m_location);
	m_chunkManager->unlockInitialization();
}
void Chunk::forceInitialization() {
	if (m_initialized) return;

	// TODO: Move elsewhere (mainly for biome stuff).
	// Noise settings.
	static bool initialized = false;
	static Noise noise = Noise(4);
	static const float noiseScale = 0.0065;
	if (!initialized) {
		noise.setOctaveCount(5);
	}

	// TODO: Base terrain settings on biome.
	// Terrain settings.
	VoxelInt range = 128;
	VoxelInt midHeight = voxelChunkSizeY / 2;

	// Get base location.
	VoxelLocation absLoc = getVoxelLocation();
	absLoc.y() -= midHeight;

	// Generate terrain.
	VoxelID voxelID;
	Layer* const tempLayers = new Layer[voxelChunkSizeY]();
	for (VoxelInt y = 0; y < voxelChunkSizeY; y++) {
		// float threshold.
		const double threshold = ((float)(absLoc.y() + y) / (float)(range));

		// Set layer data.
		for (VoxelInt x = 0; x < voxelChunkSizeX; x++) {
			for (VoxelInt z = 0; z < voxelChunkSizeZ; z++) {
				// Get noise value for location.
				const float n = noise.getNoise(
					(float)(absLoc.x() + x) * noiseScale,
					(float)(absLoc.y() + y) * noiseScale,
					(float)(absLoc.z() + z) * noiseScale
				);

				// Get voxel ID.
				if (y == 0) {
					voxelID = VoxelID::Bedrock;
				} else if (n > threshold) {
					voxelID = VoxelID::Stone;
				} else {
					voxelID = VoxelID::Air;
				}

				// Set voxelID.
				tempLayers[y].setVoxel(x, z, voxelID);
			}
		}
		// Compress layer.
		tempLayers[y].compress();
	}

	// Set layer references.
	LayerManager* const layerManager = m_chunkManager->getVoxelManager()->getLayerManager();
	layerManager->lock();
	for (VoxelInt y = 0; y < voxelChunkSizeY; y++) {
		m_layers[y] = layerManager->createLayerReference(tempLayers[y]);
	}
	layerManager->unlock();

	// Cleanup.
	delete[] tempLayers;

	// TODO: Move this elsewhere.
	// Setup basic surface.
	lockPlacement();
	for (VoxelInt x = 0; x < voxelChunkSizeX; x++) {
		for (VoxelInt z = 0; z < voxelChunkSizeZ; z++) {
			VoxelInt lastAir = voxelChunkSizeY;
			for (VoxelInt y = voxelChunkSizeY - 1; y >= 0; y--) {
				VoxelLocation location(x, y, z);

				// Check for depth.
				if (getVoxel(location) == VoxelID::Air) {
					lastAir = y;
					continue;
				}

				// Surface grass.
				if (lastAir - y == 1) {
					queueSetVoxel(location, VoxelID::Grass);
				} else if (lastAir - y <= 3) {
					queueSetVoxel(location, VoxelID::Dirt);
				}
			}
		}
	}

	// Queue placement.
	if (m_placementQueue.size() > 0) queuePlacement();
	unlockPlacement();

	// Rebuild neighbours.
	m_chunkManager->lockRebuild();
	m_chunkManager->queueRebuild(m_location);
	m_chunkManager->queueRebuild(m_location + ChunkLocation(1, 0));
	m_chunkManager->queueRebuild(m_location + ChunkLocation(-1, 0));
	m_chunkManager->queueRebuild(m_location + ChunkLocation(0, 1));
	m_chunkManager->queueRebuild(m_location + ChunkLocation(0, -1));
	m_chunkManager->unlockRebuild();

	// Update information.
	m_initialized = true;
}

void Chunk::queueTrySetVoxel(const VoxelLocation& location, const VoxelID voxelID) {
	// Make sure within chunk height.
	if (location.y() < 0 || location.y() >= voxelChunkSizeY) return;

	// Check if in another chunk.
	if (location.x() < 0 || location.z() < 0 ||
		location.x() >= voxelChunkSizeX || location.z() >= voxelChunkSizeZ) {
		// Set voxel.
		m_chunkManager->lockChunks();
		m_chunkManager->trySetVoxel((VoxelLocation)m_location + location, voxelID);
		m_chunkManager->unlockChunks();
		return;
	}

	auto it = m_placementQueue.find(location);
	if (it != m_placementQueue.end()) {
		if (it->second.m_voxelID != VoxelID::None)
			// Cannot overwrite old data.
			return;

		// Can overwrite old data.
		it = m_placementQueue.erase(it);
	}

	// Queue placement.
	m_placementQueue.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(location),
		std::forward_as_tuple(voxelID, false)
	);
}
void Chunk::queueSetVoxel(const VoxelLocation& location, const VoxelID voxelID) {
	// Make sure within chunk height.
	if (location.y() < 0 || location.y() >= voxelChunkSizeY) return;

	// Check if in another chunk.
	if (location.x() < 0 || location.z() < 0 ||
		location.x() >= voxelChunkSizeX || location.z() >= voxelChunkSizeZ) {
		// Set voxel.
		m_chunkManager->lockChunks();
		m_chunkManager->setVoxel((VoxelLocation)m_location + location, voxelID);
		m_chunkManager->unlockChunks();
		return;
	}

	// Find placement.
	auto it = m_placementQueue.find(location);
	if (it != m_placementQueue.end())
		// Overwrite old data.
		it = m_placementQueue.erase(it);

	// Queue placement.
	m_placementQueue.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(location),
		std::forward_as_tuple(voxelID, true)
	);
}
void Chunk::queuePlacement() const {
	// Queue placement.
	m_chunkManager->lockPlacement();
	m_chunkManager->queuePlacement(m_location);
	m_chunkManager->unlockPlacement();
}
void Chunk::forcePlacement() {
	if (m_placementQueue.size() <= 0 || !m_initialized) return;

	// Get changed layers.
	Map<VoxelInt, Layer> newLayers;
	for (auto it = m_placementQueue.begin(); it != m_placementQueue.end(); it++) {
		// Get place information.
		const VoxelLocation& location = it->first;
		const LayerReference& layer = m_layers[location.y()];
		const VoxelID existingVoxelID = layer.getVoxel(location.x(), location.y());
		const PlacementInformation& placementInformation = it->second;

		// Ignore if same.
		if (existingVoxelID == placementInformation.m_voxelID) continue;
		// Ignore if not forced.
		if (existingVoxelID != VoxelID::None && !placementInformation.m_force) continue;

		// Get new layer.
		auto newLayerIt = newLayers.find(location.y());
		if (newLayerIt == newLayers.end())
			// Create new new layer.
			newLayerIt = newLayers.emplace(location.y(), *layer.getVoxelLayer()).first;

		// Set new voxel.
		newLayerIt->second.setVoxel(location.x(), location.z(), placementInformation.m_voxelID);
	}
	m_placementQueue.clear();

	if (newLayers.size() > 0) {
		// Compress layers.
		for (auto it = newLayers.begin(); it != newLayers.end(); it++)
			it->second.compress();

		// Update layer references.
		LayerManager* const layerManager = m_chunkManager->getVoxelManager()->getLayerManager();
		layerManager->lock();
		for (auto it = newLayers.begin(); it != newLayers.end(); it++)
			m_layers[it->first] = layerManager->createLayerReference(it->second);
		layerManager->unlock();

		// Update rebuild.
		queueRebuild();
	}
}
void Chunk::lockPlacement() {
	m_placementMutex.lock();
}
void Chunk::unlockPlacement() {
	m_placementMutex.unlock();
}

void Chunk::queueRebuild() const {
	// Queue rebuild.
	m_chunkManager->lockRebuild();
	m_chunkManager->queueRebuild(m_location);
	m_chunkManager->unlockRebuild();
}
void Chunk::forceRebuild() {
	if (!m_initialized) return;

	// Scan & meshing data / config.
	static const VoxelLocation points[8] = {
		/* 0 */ VoxelLocation(0, 0, 0), /* 1 */ VoxelLocation(1, 0, 0),
		/* 2 */ VoxelLocation(1, 0, 1), /* 3 */ VoxelLocation(0, 0, 1),
		/* 4 */ VoxelLocation(0, 1, 0), /* 5 */ VoxelLocation(1, 1, 0),
		/* 6 */ VoxelLocation(1, 1, 1), /* 7 */ VoxelLocation(0, 1, 1),
	};
	static const VoxelInt checks[][7] = {
		{ /* Check Data */ +1, +0, +0, /* Point Indices */ 6, 5, 1, 2, },
		{ /* Check Data */ -1, -0, -0, /* Point Indices */ 4, 7, 3, 0, },
		{ /* Check Data */ +0, +1, +0, /* Point Indices */ 4, 5, 6, 7, },
		{ /* Check Data */ -0, -1, -0, /* Point Indices */ 1, 0, 3, 2, },
		{ /* Check Data */ +0, +0, +1, /* Point Indices */ 7, 6, 2, 3, },
		{ /* Check Data */ -0, -0, -1, /* Point Indices */ 5, 4, 0, 1, },
	};
	static const VoxelInt meshScanOrder[][6] = {
		{ /* Axis */ voxelChunkSizeX, voxelChunkSizeZ, voxelChunkSizeY, /* Axis order type */ 0, /* Check lookups */ 0, 1 },
		{ /* Axis */ voxelChunkSizeY, voxelChunkSizeX, voxelChunkSizeZ, /* Axis order type */ 1, /* Check lookups */ 2, 3 },
		{ /* Axis */ voxelChunkSizeZ, voxelChunkSizeX, voxelChunkSizeY, /* Axis order type */ 2, /* Check lookups */ 4, 5 },
	};
	const size_t layerSides = 2;

	// Greedy meshing data.
#	pragma pack(push, 1)
	struct LayerState {
		VoxelID m_id;

		// 33221100
		uint8_t m_ambience;

		J_GETTER_DIRECT(isEnabled, m_id != VoxelID::None, bool);
		const bool operator ==(const LayerState& other) {
			// Compare id.
			if (m_id != other.m_id) return false;
			// Compare ambience.
			if (m_ambience != other.m_ambience) return false;

			// The same.
			return true;
		}
		inline const float getAmbience(const uint8_t index) {
			static constexpr float ambienceFactor = 0.74f;

			// Find ambience information for index.
			switch ((*this)[index]) {
				case 1: return ambienceFactor;
				case 2: return ambienceFactor * ambienceFactor;
				case 3: return ambienceFactor * ambienceFactor * ambienceFactor;
			}

			// No ambience information found for index.
			return 1.0f;
		}
		const bool shouldFlipAmbience() {
			return
				((*this)[1] > (*this)[0] && (*this)[1] > (*this)[2]) ||
				((*this)[3] > (*this)[0] && (*this)[3] > (*this)[2]);
		}

		inline const uint8_t operator[](const uint8_t index) const {
			return (m_ambience >> (index * 2)) & 0b11;
		}
	};
#	pragma pack(pop)

	// Mesh storage.
	List<VoxelLocation> voxelVertices;
	List<Vector3> colors;
	List<uint32_t> indices;

	// It's Meshin' time.
	bool ambience[8];
	VoxelLocation location;
	for (size_t s = 0; s < sizeof(meshScanOrder) / sizeof(*meshScanOrder); s++) {
		const VoxelInt(&scan)[6] = meshScanOrder[s];

		// Create layer data.
		const size_t layerSize = scan[1] * scan[2];
		LayerState* const layerState = new LayerState[layerSize * 2];

		for (VoxelInt a = 0; a < scan[0]; a++) {
			// Pre-process layer.
			for (VoxelInt b = 0; b < scan[1]; b++) {
				for (VoxelInt c = 0; c < scan[2]; c++) {
					// Get location.
					switch (scan[3]) {
						case 0: location.x() = a; location.z() = b; location.y() = c; break;
						case 1: location.y() = a; location.x() = b; location.z() = c; break;
						case 2: location.z() = a; location.x() = b; location.y() = c; break;
					}

					// Check if voxel solid.
					const VoxelID voxel = getVoxel(location);
					if (voxel == VoxelID::None) {
						layerState[b + (c * scan[1])].m_id = VoxelID::None;
						layerState[b + (c * scan[1]) + layerSize].m_id = VoxelID::None;
						continue;
					}

					// Set state id.
					layerState[b + (c * scan[1])].m_id = voxel;
					layerState[b + (c * scan[1]) + layerSize].m_id = voxel;

					for (size_t layerSide = 0; layerSide < layerSides; layerSide++) {
						// Get state information.
						const size_t layerOffset = (layerSide * layerSize);
						LayerState& activeState = layerState[b + (c * scan[1]) + layerOffset];

						// Get ambience information.
						const VoxelInt(&check)[7] = checks[scan[layerSide + 4]];
						// Ignore if voxel disabled.
						const VoxelID checkVoxel = getVoxel(location + VoxelLocation(check[0], check[1], check[2]));
						if (checkVoxel != VoxelID::None) {
							activeState.m_id = VoxelID::None;
							continue;
						}

						// Get ambience information.
						if (check[0]) {
							ambience[0] = getVoxel(location + VoxelLocation(check[0], 1, 0)) != VoxelID::None;
							ambience[1] = getVoxel(location + VoxelLocation(check[0], 0, -check[0])) != VoxelID::None;
							ambience[2] = getVoxel(location + VoxelLocation(check[0], -1, 0)) != VoxelID::None;
							ambience[3] = getVoxel(location + VoxelLocation(check[0], 0, check[0])) != VoxelID::None;
							ambience[4] = getVoxel(location + VoxelLocation(check[0], +1, check[0])) != VoxelID::None;
							ambience[5] = getVoxel(location + VoxelLocation(check[0], +1, -check[0])) != VoxelID::None;
							ambience[6] = getVoxel(location + VoxelLocation(check[0], -1, -check[0])) != VoxelID::None;
							ambience[7] = getVoxel(location + VoxelLocation(check[0], -1, check[0])) != VoxelID::None;
						} else if (check[1]) {
							ambience[0] = getVoxel(location + VoxelLocation(0, check[1], -1)) != VoxelID::None;
							ambience[1] = getVoxel(location + VoxelLocation(check[1], check[1], 0)) != VoxelID::None;
							ambience[2] = getVoxel(location + VoxelLocation(0, check[1], 1)) != VoxelID::None;
							ambience[3] = getVoxel(location + VoxelLocation(-check[1], check[1], 0)) != VoxelID::None;
							ambience[4] = getVoxel(location + VoxelLocation(-check[1], check[1], -1)) != VoxelID::None;
							ambience[5] = getVoxel(location + VoxelLocation(check[1], check[1], -1)) != VoxelID::None;
							ambience[6] = getVoxel(location + VoxelLocation(check[1], check[1], 1)) != VoxelID::None;
							ambience[7] = getVoxel(location + VoxelLocation(-check[1], check[1], 1)) != VoxelID::None;
						} else {
							ambience[0] = getVoxel(location + VoxelLocation(0, 1, check[2])) != VoxelID::None;
							ambience[1] = getVoxel(location + VoxelLocation(check[2], 0, check[2])) != VoxelID::None;
							ambience[2] = getVoxel(location + VoxelLocation(0, -1, check[2])) != VoxelID::None;
							ambience[3] = getVoxel(location + VoxelLocation(-check[2], 0, check[2])) != VoxelID::None;
							ambience[4] = getVoxel(location + VoxelLocation(-check[2], 1, check[2])) != VoxelID::None;
							ambience[5] = getVoxel(location + VoxelLocation(check[2], 1, check[2])) != VoxelID::None;
							ambience[6] = getVoxel(location + VoxelLocation(check[2], -1, check[2])) != VoxelID::None;
							ambience[7] = getVoxel(location + VoxelLocation(-check[2], -1, check[2])) != VoxelID::None;
						};

						// Set ambient information.
						activeState.m_ambience = 0;
						if (ambience[0]) activeState.m_ambience += 1;
						if (ambience[3]) activeState.m_ambience += 1;
						if (ambience[4]) activeState.m_ambience += 1;
						if (ambience[0]) activeState.m_ambience += 1 << 2;
						if (ambience[1]) activeState.m_ambience += 1 << 2;
						if (ambience[5]) activeState.m_ambience += 1 << 2;
						if (ambience[1]) activeState.m_ambience += 1 << 4;
						if (ambience[2]) activeState.m_ambience += 1 << 4;
						if (ambience[6]) activeState.m_ambience += 1 << 4;
						if (ambience[2]) activeState.m_ambience += 1 << 6;
						if (ambience[3]) activeState.m_ambience += 1 << 6;
						if (ambience[7]) activeState.m_ambience += 1 << 6;
					}
				}
			}

			// Greedy mesh (Checks through layers).
			for (size_t layerSide = 0; layerSide < layerSides; layerSide++) {
				// Get layer-associated information
				const VoxelInt(&check)[7] = checks[scan[layerSide + 4]];
				LayerState* const layerData = layerState + (layerSide * layerSize);

				// Create faces.
				size_t faceWidth, faceHeight;
				for (VoxelInt c = 0; c < scan[2]; c++) {
					for (VoxelInt b = 0; b < scan[1]; b++) {
						const size_t layerIndex = b + (c * scan[1]);

						// Check if state active.
						LayerState& activeState = layerData[layerIndex];
						if (!activeState.isEnabled()) continue;

						// Initialize base size.
						faceWidth = 1;
						faceHeight = 1;

						// Check to the B axis (right).
						while (b + faceWidth < scan[1] && activeState == layerData[layerIndex + faceWidth]) {
							layerData[layerIndex + faceWidth].m_id = VoxelID::None;
							faceWidth++;
						}

						// Check on the C axis (down).
						bool faceCanIncrement = true;
						while (faceCanIncrement && c + faceHeight < scan[2]) {
							for (VoxelInt i = 0; faceCanIncrement && i < faceWidth; i++) {
								const size_t index = layerIndex + i + (scan[1] * faceHeight);
								// Check if face can be used.
								if (!(activeState == layerData[index])) faceCanIncrement = false;
							}

							// Check state.
							if (!faceCanIncrement) continue;

							// Update states.
							for (VoxelInt i = 0; faceCanIncrement && i < faceWidth; i++) {
								const size_t index = layerIndex + i + (scan[1] * faceHeight);
								layerData[index].m_id = VoxelID::None;
							}

							// Increment face height.
							faceHeight++;
						}

						// Update base state.
						activeState.m_id = VoxelID::None;

						// Get face vertex information.
						VoxelLocation faceMultiplier(1, 1, 1);
						switch (scan[3]) {
							case 0: {
								location.x() = a;
								location.z() = b;
								location.y() = c;
								faceMultiplier.z() = faceWidth;
								faceMultiplier.y() = faceHeight;
							} break;
							case 1: {
								location.y() = a;
								location.x() = b;
								location.z() = c;
								faceMultiplier.x() = faceWidth;
								faceMultiplier.z() = faceHeight;
							} break;
							case 2: {
								location.z() = a;
								location.x() = b;
								location.y() = c;
								faceMultiplier.x() = faceWidth;
								faceMultiplier.y() = faceHeight;
							} break;
						}

						// Push vertices.
						const uint32_t io = voxelVertices.size();
						voxelVertices.push_back(location + (points[check[3]] * faceMultiplier));
						voxelVertices.push_back(location + (points[check[4]] * faceMultiplier));
						voxelVertices.push_back(location + (points[check[5]] * faceMultiplier));
						voxelVertices.push_back(location + (points[check[6]] * faceMultiplier));

						// Push colors.
						const Vector3& color = getVoxelColor(getVoxel(location));
						colors.push_back(color * activeState.getAmbience(0));
						colors.push_back(color * activeState.getAmbience(1));
						colors.push_back(color * activeState.getAmbience(2));
						colors.push_back(color * activeState.getAmbience(3));

						// Push indices.
						if (activeState.shouldFlipAmbience()) {
							// CW ordering starting from index 1.
							indices.push_back(io + 1);
							indices.push_back(io + 2);
							indices.push_back(io + 0);
							indices.push_back(io + 3);
							indices.push_back(io + 0);
							indices.push_back(io + 2);
						} else {
							// CW ordering starting from index 0.
							indices.push_back(io + 0);
							indices.push_back(io + 1);
							indices.push_back(io + 3);
							indices.push_back(io + 2);
							indices.push_back(io + 3);
							indices.push_back(io + 1);
						}
					}
				}
			}
		}
		delete[] layerState;
	}

	// Generate packed vertices.
	List<uint32_t> packedVertices;
	for (size_t i = 0; i < voxelVertices.size() && i < colors.size(); i++) {
		// 00000000000000000000000000000000
		// rrrrgggggbbbbzzzzzyyyyyyyyyxxxxx
		packedVertices.push_back(
			// Vertex information.
			((voxelVertices[i].x() & 0x1f) << 0) |
			((voxelVertices[i].y() & 0x1ff) << 5) |
			((voxelVertices[i].z() & 0x1f) << 14) |
			// Color information.
			(((uint32_t)(colors[i].x() * (float)0xf) & 0xf) << 19) |
			(((uint32_t)(colors[i].y() * (float)0x1f) & 0x1f) << 23) |
			(((uint32_t)(colors[i].z() * (float)0xf) & 0xf) << 28)
		);
	}

	// Get mesh.
	m_model.lock();
	ModelMesh& mesh = m_model.getInactiveMesh();
	mesh.lock();

	// Set mesh data.
	mesh.set("v_packedVertex", packedVertices);
	mesh.setIndices(indices);

	// Unlock.
	mesh.unlock();
	m_model.unlock();
}

void Chunk::queueDraw() const {
	// Queue draw.
	m_chunkManager->lockDraw();
	m_chunkManager->queueDraw(m_location);
	m_chunkManager->unlockDraw();
}
void Chunk::forceDraw(InstanceData& instanceData) {
	if (!m_initialized || m_drawn) return;

	// Set instance data.
	const VoxelLocation location = getVoxelLocation();
	instanceData.setData("i_position", Vector3(
		location.x(), location.y(), location.z()
	));

	// Draw chunk (statically).
	Draw.draw(m_model, instanceData, this);

	// Get mesh.
	m_model.lock();
	ModelMesh& mesh = m_model.getActiveMesh();
	mesh.lock();
	// Check if mesh has been made.
	if (mesh.getRenderCount() <= 0)
		queueRebuild();
	// Unlock.
	mesh.unlock();
	m_model.unlock();

	// Update details.
	m_drawn = true;
}

void Chunk::queueUndraw() const {
	// Queue undraw.
	m_chunkManager->lockUndraw();
	m_chunkManager->queueUndraw(m_location);
	m_chunkManager->unlockUndraw();
}
void Chunk::forceUndraw() {
	if (!m_drawn) return;

	// Undraw chunk.
	Draw.undrawStatic(this);

	// Get mesh.
	m_model.lock();
	ModelMesh& mesh = m_model.getActiveMesh();
	mesh.lock();
	// Clear mesh (its for the best).
	mesh.clear();
	// Unlock.
	mesh.unlock();
	m_model.unlock();

	// Update details.
	m_drawn = false;
}