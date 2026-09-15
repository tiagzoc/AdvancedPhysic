#pragma once

#include "framework.h"

#include "Entity.h"

class Entity;

struct CollisionCheck
{

	static bool Collide(Entity* pEntity1, Entity* pEntity2, Impact& impact);

private:
	using pCollideFunc = bool(*)(Entity* pEntity1, Entity* pEntity2, Impact& impact);

	static bool CollideSphere_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideSphere_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideSphere_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact);
		   
	static bool CollideAABB_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideAABB_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideAABB_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact);
		   
	static bool CollideOBB_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideOBB_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact);
	static bool CollideOBB_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact);

	inline static pCollideFunc CollideFunc[(uint64_t)Collider::ColliderType::COUNT][(uint64_t)Collider::ColliderType::COUNT] =
	{  // Sphere				AABB				OBB
		{ CollideSphere_Sphere, CollideSphere_AABB, CollideSphere_OBB },	// Sphere
		{ CollideAABB_Sphere,	CollideAABB_AABB,	CollideAABB_OBB },		// AABB
		{ CollideOBB_Sphere,	CollideOBB_AABB,	CollideOBB_OBB }		// OBB
	};

};