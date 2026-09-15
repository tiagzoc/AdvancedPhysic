#pragma once

#include "framework.h"

#include "Transform.h"


enum class Shape
{
	CUBE_AABB,
	CUBE_OBB,
	SPHERE
};

class Entity;

struct Impact
{	
	// the point of contact between the two objects colliding at object 1
	glm::vec3 collision_point_local_1; 

	// the point of contact between the two objects colliding at object 2
	glm::vec3 collision_point_local_2;

	// the normal to the collision
	glm::vec3 normal;

	// the offset to add to the position to get out of the collision
	float penetration;

	Entity* pEntity1;
	Entity* pEntity2;

	Impact(Entity* pentity1, Entity* pentity2) :
		pEntity1(pentity1),
		pEntity2(pentity2)
	{ }
};

struct ImpactResponse
{
	glm::vec3 Offset;
	glm::vec3 Force;
};

struct Collider
{
	enum class ColliderType : uint64_t
	{
		SPHERE,
		AABB,
		OBB,

		COUNT,
	};

	Transform* pTransform;
	ColliderType Type;

	Collider(Transform* ptransform, ColliderType type) : 
		Type(type),
		pTransform(ptransform) 
	{ }

	glm::vec3 GetHalfSize();
};