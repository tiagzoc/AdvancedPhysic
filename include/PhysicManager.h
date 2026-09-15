#pragma once


#include "framework.h"
#include "AssetManager.h"
#include "Entity.h"

#include "Collider.h"
#include "RigidBody.h"
#include "CollisionCheck.h"

#include "ComputeShader.h"

#include <vector>
#include <list>

#include "InputManager.h"

static constexpr uint64_t MAX_ENTITIES = 1280000 / 112;
static constexpr uint64_t MAX_IMPACTS = 12800000 / 64;


struct alignas(16) Collider_GPU
{
	glm::mat4 rotation;
	glm::vec4 position;
	glm::vec4 half_extent;
	glm::ivec4 cell_coords;
	uint32_t entity_index;
	uint32_t type;
	int PAD[2];
};

struct alignas(16) Impact_GPU
{
	glm::vec4 local_collision_point_1;
	glm::vec4 local_collision_point_2;
	glm::vec4 normal;
	float penetration;
	uint32_t entity_index1;
	uint32_t entity_index2;
	int PAD;
};

class PhysicManager {

	std::vector<Entity*> mpEntities;
	std::queue<Impact> mImpactQueue;
	Transform* mpBoundingBox;

	MeshRenderable mSpacePatritioningWireframe;

	ComputeProgram mPhysicComputeProgram;
	ComputeShader mPhysicComputeShader;
	std::vector<Collider_GPU> mColliders;

	ReadBuffer1D<Collider_GPU>* mpCollider_GPU;
	WriteBuffer1D<Impact_GPU>* mpImpact_GPU;
	AtomicBuffer<uint32_t>* mpAtomic;

	TMM::ThreadList<Impact_GPU> mUpdateImpacts;
	TMM::ThreadList<Collider_GPU> mUpdateColliders;

	glm::uvec3 mSpacePartitioningCount;
	glm::vec3 mSpacePartitioningSize;

	bool mShowMeshs;
	bool mShowWireframes;
	bool mShowPointeds;
	bool mShowSpacePartitioning;

	void ResolveImpact(Impact_GPU& edited_data, uint64_t index_edited, const Impact_GPU* unsafe_resource, uint64_t resouce_size);
	void UpdateCollider(Collider_GPU& edited_data, uint64_t index_edited, const Collider_GPU* unsafe_resource, uint64_t resouce_size);

	void UpdateCollision();
	void ResetCollider();
	//void CheckForImpact(Entity* pEntity1, Entity* pEntity2);

	void UpdateImpacts(float dt);

	glm::uvec3 PositionToCellCoord(glm::vec3 pos);

	void UpdateRigidBody(Entity* pEntity, float dt);
	void CheckForInBoundingBox(Entity* pEntity);
public:

	PhysicManager();
	~PhysicManager();

	void Init(Transform* pBoundingBox, glm::uvec3 space_partitioning);

	Entity* AddEntity(Shape shape = Shape::CUBE_AABB, bool addCollider = true, bool addRigidBody = true);

	Entity* GetEntity(unsigned int i) { return mpEntities[i]; }
	unsigned int GetEntityCount() { return mpEntities.size(); }

	void ToggleMeshVisibility();
	void ToggleWireframeVisibility();
	void TogglePointsVisibility();
	void ToggleSpacePartitioningVisibility();

	void Update(UpdateCtx* pCtx);
	void Render(UpdateCtx* pCtx);

	void Destroy();
};