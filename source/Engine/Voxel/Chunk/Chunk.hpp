#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Voxel/Types/VoxelLocation.hpp>
#include <Engine/Voxel/Types/ChunkLocation.hpp>
#include <Engine/Voxel/Layer/LayerReference.hpp>

#include <Engine/Draw/Model.hpp>
#include <Engine/Draw/InstanceData.hpp>

class ChunkManager;

class Chunk {
private:
	ChunkManager* m_chunkManager;

private:
	const ChunkLocation m_location;
	Mutex m_detailsMutex;
	bool m_initialized, m_hasMesh, m_drawn;

private:
	LayerReference* m_layers;
	struct PlacementInformation {
		VoxelID m_voxelID;
		bool m_force;
		PlacementInformation(const VoxelID voxelID, const bool force);
	};
	Mutex m_placementMutex;
	Map<VoxelLocation, PlacementInformation> m_placementQueue;

private:
	Model m_model;

public:
	Chunk(ChunkManager* const chunkManager, const ChunkLocation& location);
	Chunk(const Chunk& other) = delete;
	~Chunk();

public:
	J_GETTER_DIRECT(isInitialized, m_initialized, bool);
	J_GETTER_DIRECT(isVisible, m_drawn, bool);
	const VoxelID getVoxel(const VoxelLocation& location);
	J_GETTER_DIRECT(getChunkLocation, m_location, ChunkLocation&);
	J_GETTER_DIRECT(getVoxelLocation, m_location, VoxelLocation);
	J_GETTER_DIRECT_MUT(getModel, &m_model, Model* const);

public:
	void lockDetails();
	void unlockDetails();

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

public:
	void queueUndraw() const;
	void forceUndraw();
};