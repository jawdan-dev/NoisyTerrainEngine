#pragma once
#include <NoisyTerrain/Core/Core.hpp>

#include <Voxel/Types/VoxelLocation.hpp>
#include <Voxel/Types/ChunkLocation.hpp>
#include <Voxel/Layer/LayerReference.hpp>

#include <NoisyTerrain/Draw/Model.hpp>
#include <NoisyTerrain/Draw/InstanceData.hpp>

class ChunkManager;

class Chunk {
private:
	ChunkManager* m_chunkManager;

private:
	bool m_initialized : 1, m_drawn : 1;
	ChunkLocation m_location;
	LayerReference* m_layers;
	Model m_model;

private:
	struct PlacementInformation {
		VoxelID m_voxelID;
		bool m_force;
		PlacementInformation(const VoxelID voxelID, const bool force);
	};
	Mutex m_placementMutex;
	Map<VoxelLocation, PlacementInformation> m_placementQueue;

public:
	Chunk(ChunkManager* const chunkManager, const ChunkLocation& location);
	Chunk(const Chunk& other) = delete;
	~Chunk();

public:
	J_GETTER_DIRECT(hasLoaded, m_initialized, bool);
	const VoxelID getVoxel(const VoxelLocation& location);
	J_GETTER_DIRECT(getChunkLocation, m_location, ChunkLocation&);
	J_GETTER_DIRECT(getVoxelLocation, m_location, VoxelLocation);
	J_GETTER_DIRECT_MUT(getModel, &m_model, Model* const);

public:
	void queueInitialization() const;
	void forceInitialization();

public:
	void queueTrySetVoxel(const VoxelLocation& location, const VoxelID voxelID);
	void queueSetVoxel(const VoxelLocation& location, const VoxelID voxelID);
	void lockPlacement();
	void unlockPlacement();
	void queuePlacement() const;
	void forcePlacement();

public:
	void queueRebuild() const;
	void forceRebuild();

public:
	void queueDraw() const;
	void forceDraw(InstanceData& instanceData);
	void forceUndraw();
};