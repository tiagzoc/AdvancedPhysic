#include "Transform.h"

void Transform::UpdateRotationMatrix()
{
	if (mIsRotationDirty == false) return;

	mRotationMatrix = RotateYawPitchRoll(mRotation);
	mInvRotationMatrix = glm::inverse(mRotationMatrix);

	mRight		= { mRotationMatrix[0][0], mRotationMatrix[0][1], mRotationMatrix[0][2] };
	mUp			= { mRotationMatrix[1][0], mRotationMatrix[1][1], mRotationMatrix[1][2] };
	mForward	= { mRotationMatrix[2][0], mRotationMatrix[2][1], mRotationMatrix[2][2] };

	mIsRotationDirty = false;
}


void Transform::UpdateTransform() {
	if (mIsDirty == false) return;

	glm::mat4 T = Translate(mPosition);
	glm::mat4 S = Scale(mScale);
	mTransform = T * mRotationMatrix * S;
	mIsDirty = false;

}

void Transform::UpdateInvTransform()
{
	if (mIsInvDirty == false) return;

	mInvTransform = glm::inverse(mTransform);
	mIsInvDirty = false;
}

Transform::Transform() :
	mPosition(0, 0, 0),
	mScale(1, 1, 1),
	mRotation(0, 0, 0),
	mIsDirty(true),
	mIsInvDirty(true)
{

}

const glm::vec3& Transform::GetPosition() const
{
	return mPosition;
}

float Transform::GetX(float x) const
{
	return mPosition.x;
}

float Transform::GetY(float y) const
{
	return mPosition.y;
}

float Transform::GetZ(float z) const
{
	return mPosition.z;
}

const glm::mat4& Transform::GetMatrixRotation() 
{
	UpdateRotationMatrix();
	return mRotationMatrix;
}

const glm::mat4& Transform::GetInverseMatrixRotation()
{
	UpdateRotationMatrix();
	return mInvRotationMatrix;
}

const glm::vec3& Transform::GetRotation() const
{
	return mRotation;
}

float Transform::GetYaw() const
{
	return mRotation.x;
}

float Transform::GetPitch() const
{
	return mRotation.y;
}

float Transform::GetRoll() const
{
	return mRotation.z;
}

float Transform::GetMaxScale() const
{
	return std::max(mScale.x, std::max(mScale.y, mScale.z));
}

float Transform::GetMinScale() const
{
	return std::min(mScale.x, std::min(mScale.y, mScale.z));
}

const glm::vec3& Transform::GetScale() const
{
	return mScale;
}

const glm::vec3& Transform::GetRight()
{
	UpdateRotationMatrix();
	return mRight;
}

const glm::vec3& Transform::GetUp()
{
	UpdateRotationMatrix();
	return mUp;
}

const glm::vec3& Transform::GetForward()
{
	UpdateRotationMatrix();
	return mForward;
}

glm::mat4& Transform::GetMatrix()
{
	UpdateRotationMatrix();
	UpdateTransform();
	return mTransform;
}

glm::mat4& Transform::GetInvMatrix()
{
	UpdateRotationMatrix();
	UpdateTransform();
	UpdateInvTransform();
	return mInvTransform;
}

void Transform::SetPosition(glm::vec3 pos)
{
	mPosition = pos;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::SetX(float x)
{
	mPosition.x = x;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::SetY(float y)
{
	mPosition.y = y;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::SetZ(float z)
{
	mPosition.z = z;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::SetRotation(glm::vec3 rot)
{
	mRotation = rot;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::SetYaw(float yaw)
{
	mRotation.x = yaw;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::SetPitch(float pitch)
{
	mRotation.y = pitch;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::SetRoll(float roll)
{
	mRotation.z = roll;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::SetScale(glm::vec3 scale)
{
	mScale = scale;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::AddPosition(glm::vec3 pos)
{
	mPosition += pos;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::AddX(float x)
{
	mPosition.x += x;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::AddY(float y)
{
	mPosition.y += y;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::AddZ(float z)
{
	mPosition.z += z;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::AddRotation(glm::vec3 rot)
{
	mRotation += rot;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::AddYaw(float yaw)
{
	mRotation.x += yaw;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::AddPitch(float pitch)
{
	mRotation.y += pitch;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::AddRoll(float roll)
{
	mRotation.z += roll;
	mIsDirty = true;
	mIsInvDirty = true;
	mIsRotationDirty = true;
}

void Transform::AddScale(glm::vec3 scale)
{
	mScale += scale;
	mIsDirty = true;
	mIsInvDirty = true;
}

void Transform::ApplyTransform(Transform& transform)
{
	mRotationMatrix = transform.GetMatrixRotation() * GetMatrixRotation();
	mTransform = transform.GetMatrix() * GetMatrix();
	mIsInvDirty = true;
}
