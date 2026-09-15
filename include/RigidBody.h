#pragma once

#include "framework.h"
#include "TMM_Functional.h"
#include "Transform.h"
#include "Collider.h"
#include "SafeQueue.h"

class RigidBody
{
public:

	using ComputeVelocity = TMM::Callable<glm::vec3, const glm::vec3&>*;
	using ComputeAcceleration = TMM::Callable<glm::vec3, const glm::vec3&>*;

	static constexpr float DEFAULT_GRAVITY = 9.81f;
	static constexpr float DEFAULT_AIR_VISCOSITY = 0.01f;
	static constexpr float DEFAULT_COEF_FRICTION = 0.3f;
	static constexpr float DEFAULT_COEF_FRICTION_BOUNDING_BOX = 0.9f;

	inline static float Gravity = DEFAULT_GRAVITY;
	inline static float Air_Viscosity = DEFAULT_AIR_VISCOSITY;
	inline static float Coef_Friction = DEFAULT_COEF_FRICTION;
	inline static float Coef_Friction_Bounding_Box = DEFAULT_COEF_FRICTION_BOUNDING_BOX;

private:

	Transform* pTransform;
	float mMass;
	float mInvMass;
	glm::vec3 mVelocity;
	glm::vec3 mAcceleration;

	SafeQueue<glm::vec3> mForces;
	SafeQueue<glm::vec3> mVelocityBounce;
	SafeQueue<glm::vec3> mOffsets;

	void UpdateAcceleration(float dt);
	void UpdateVelocity(float dt);
	void UpdatePosition(float dt);

public:
	RigidBody(Transform* pEntity, float mass);

	void SetMass(float mass) { mMass = mass; }
	float GetMass() const { return mMass; }
	float GetInvMass() const { return mInvMass; }

	void BounceBoundingBox(const glm::ivec3& dir);
	void SetVelocity(glm::vec3 vel);

	void AddForce(const glm::vec3& force);
	void AddVelocityBounce(const glm::vec3& force);
	void AddOffset(const glm::vec3& offset);

	const glm::vec3& GetVelocity() const;

	void Update(float dt);
};