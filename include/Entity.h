#pragma once

#include "framework.h"

#include "Transform.h"
#include "Renderable.h"
#include "Shader.h"

#include "Collider.h"
#include "RigidBody.h"

#include "AssetManager.h"


struct UpdateCtx
{
	const glm::mat4* pView;
	const glm::mat4* pProj;
	const glm::mat4* pVP;
	double deltaTime;
	double time;
	unsigned int physic_step_count;
};

class Entity
{
	Transform mTransform;

	MeshRenderable mMesh;
	MeshRenderable mWireColliderMesh;
	MeshRenderable mPointedMesh;

	bool mShowMesh;
	bool mShowWireCollider;
	bool mShowPointedMesh;

	float mMaxRenderDistanceDebug = 10.0f;

	Collider* mpCollider;
	RigidBody* mpRigidBody;

	Texture* mpTex;

	glm::vec3 mDirection;

	float mSpeed;

public:
	// Debug Only
	bool Is_Colliding;

	Entity(RenderProgram* pWireframe, RenderProgram* pUnlit, Mesh* pMesh);

	void SetShowMesh(bool active) { mShowMesh = active; }
	void SetShowWireCollider(bool active) { mShowWireCollider = active; }
	void SetShowPointed(bool active) { mShowPointedMesh = active; }

	bool GetShowMesh() { return  mShowMesh; }
	bool GetShowWireCollider() { return  mShowWireCollider; }
	bool GetShowPointed() { return  mShowPointedMesh; }

	void SetMaxRenderDistanceDebug(float distance) { mMaxRenderDistanceDebug = distance; }

	void CreateCollider(Shape shape);
	Collider* GetCollider();

	void CreateRigidBody();
	RigidBody* GetRigidBody();

	void Render(UpdateCtx* pCtx);
	void RenderWireCollider(UpdateCtx* pCtx);
	void RenderPointedMesh(UpdateCtx* pCtx);

	Transform& GetTransform() { return mTransform; }
	glm::vec3 GetBoundingBox();
};