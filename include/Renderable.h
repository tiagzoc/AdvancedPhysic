#pragma once

#include "framework.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

enum RenderMethod
{
	TRIANGLE = GL_FILL,
	LINE = GL_LINE,
	P = GL_POINT
};

__interface Renderable
{
	virtual RenderProgram* ActivateRenderProgram() = 0;
	virtual void UseRenderProgram(RenderProgram* pProgram) = 0;

	virtual void UseMesh(Mesh* pMesh) = 0;

	virtual RenderMethod UsedRenderMethod() = 0;
	virtual bool UsedDepth() = 0;

	virtual void SetRenderMethod(RenderMethod method) = 0;
	virtual void SetDepthUsage(bool active) = 0;

	virtual void Init(RenderMethod useMethod, bool useDepth) = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;
};

class MeshRenderable : public Renderable
{

	RenderProgram* mpRenderProgram;
	Mesh* mpMesh;

	bool mUseDepth;
	RenderMethod mRenderMethod;

public:
	MeshRenderable(RenderProgram* pProgram, Mesh* pMesh);
	virtual ~MeshRenderable();

	virtual RenderProgram* ActivateRenderProgram() override;
	virtual void UseRenderProgram(RenderProgram* pProgram) override;

	virtual void UseMesh(Mesh* pMesh) override;

	virtual RenderMethod UsedRenderMethod() override;
	virtual bool UsedDepth() override;

	virtual void SetRenderMethod(RenderMethod method) override;
	virtual void SetDepthUsage(bool active) override;

	virtual void Init(RenderMethod useMethod = RenderMethod::TRIANGLE, bool useDepth = true) override;
	virtual void Draw() override;
	virtual void Destroy() override;

};