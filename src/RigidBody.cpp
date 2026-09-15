#include "RigidBody.h"

void RigidBody::UpdateAcceleration(float dt)
{
	float air_resistance = 6 * glm::pi<float>() * Air_Viscosity * pTransform->GetMaxScale() * mInvMass;

	if (mForces.Size() > 0)
	{
		int a = 0;
	}

	glm::vec3 total_forces = { 0, 0, 0 };
	while (mForces.Size() > 0)
	{
		total_forces += mForces.Get();
		mForces.Pop();
	}

	mAcceleration = glm::vec3{ 0, -Gravity, 0 } - mVelocity * air_resistance + total_forces * mInvMass;
}

void RigidBody::UpdateVelocity(float dt)
{
	if (mVelocityBounce.Size() > 0)
	{
		int a = 0;
	}

	glm::vec3 total_bounce = { 0, 0, 0 };
	while (mVelocityBounce.Size() > 0)
	{
		total_bounce += mVelocityBounce.Get();
		mVelocityBounce.Pop();
	}

	mVelocity += mAcceleration * dt + total_bounce * mInvMass;
}

void RigidBody::UpdatePosition(float dt)
{
	if (mOffsets.Size() > 0)
	{
		int a = 0;
	}

	glm::vec3 total_offset = { 0, 0, 0 };
	while (mOffsets.Size() > 0)
	{
		total_offset += mOffsets.Get();
		mOffsets.Pop();
	}

	pTransform->AddPosition(
		mVelocity * dt + total_offset
	);
}

RigidBody::RigidBody(Transform* pEntity, float mass) :
	pTransform(pEntity),
	mMass(mass),
	mInvMass(1 / mass),
	mVelocity(0, 0, 0),
	mAcceleration(0, 0, 0)
{

}

void RigidBody::BounceBoundingBox(const glm::ivec3& dir)
{

	mVelocity = {
		(dir.x == 0 ? mVelocity.x : (dir.x < 0 ? -abs(mVelocity.x) : abs(mVelocity.x))) * Coef_Friction_Bounding_Box,
		(dir.y == 0 ? mVelocity.y : (dir.y < 0 ? -abs(mVelocity.y) : abs(mVelocity.y))) * Coef_Friction_Bounding_Box,
		(dir.z == 0 ? mVelocity.z : (dir.z < 0 ? -abs(mVelocity.z) : abs(mVelocity.z))) * Coef_Friction_Bounding_Box
	};

}

void RigidBody::SetVelocity(glm::vec3 vel)
{
	mVelocity = vel;
}

void RigidBody::AddForce(const glm::vec3& force)
{
	mForces.Push(force);
}

void RigidBody::AddVelocityBounce(const glm::vec3& force)
{
	mVelocityBounce.Push(force);
}

void RigidBody::AddOffset(const glm::vec3& offset)
{
	mOffsets.Push(offset);
}

const glm::vec3& RigidBody::GetVelocity() const
{
	return mVelocity;
}

void RigidBody::Update(float dt)
{
	//std::cout << "Velocity : " << mVelocity.x << ", " << mVelocity.y << ", " << mVelocity.z << std::endl;
	UpdateAcceleration(dt);
	UpdateVelocity(dt);
	UpdatePosition(dt);
}
