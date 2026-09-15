#pragma once

#include "framework.h"
#include "MatTransform.h"

class Transform
{
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;

	glm::vec3 mRight;
	glm::vec3 mUp;
	glm::vec3 mForward;

	glm::mat4 mRotationMatrix;
	glm::mat4 mInvRotationMatrix;
	glm::mat4 mTransform;
	glm::mat4 mInvTransform;

	bool mIsDirty;
	bool mIsRotationDirty;
	bool mIsInvDirty;

	void UpdateRotationMatrix();
	void UpdateTransform();
	void UpdateInvTransform();
public:

	Transform();

	const glm::vec3&  GetPosition() const;
	float GetX(float x) const;
	float GetY(float y) const;
	float GetZ(float z) const;
	const glm::mat4& GetMatrixRotation();
	const glm::mat4& GetInverseMatrixRotation();
	const glm::vec3&  GetRotation() const;
	float GetYaw() const;
	float GetPitch() const;
	float GetRoll() const;
	float GetMaxScale() const;
	float GetMinScale() const;
	const glm::vec3& GetScale() const;
	const glm::vec3& GetRight();
	const glm::vec3& GetUp();
	const glm::vec3& GetForward();
	
	glm::mat4& GetMatrix();
	glm::mat4& GetInvMatrix();

	bool GetDirty() { return mIsDirty; }

	void SetPosition(glm::vec3 pos);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetRotation(glm::vec3 rot);
	void SetYaw(float yaw);
	void SetPitch(float pitch);
	void SetRoll(float roll);
	void SetScale(glm::vec3 scale);

	void AddPosition(glm::vec3 pos);
	void AddX(float x);
	void AddY(float y);
	void AddZ(float z);
	void AddRotation(glm::vec3 rot);
	void AddYaw(float yaw);
	void AddPitch(float pitch);
	void AddRoll(float roll);
	void AddScale(glm::vec3 scale);

	void ApplyTransform(Transform& transform);
};