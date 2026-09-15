#include "Renderable.h"

MeshRenderable::MeshRenderable(RenderProgram* pProgram, Mesh* pMesh) :
	mpRenderProgram(pProgram),
	mpMesh(pMesh),
	mUseDepth(true),
	mRenderMethod(RenderMethod::TRIANGLE)
{

}

MeshRenderable::~MeshRenderable()
{
	Destroy();
}

RenderProgram* MeshRenderable::ActivateRenderProgram()
{
	mpRenderProgram->SetActive();
	return mpRenderProgram;
}

void MeshRenderable::UseRenderProgram(RenderProgram* pProgram)
{
	mpRenderProgram = pProgram;
}

void MeshRenderable::UseMesh(Mesh* pMesh)
{
	mpMesh = pMesh;
}

RenderMethod MeshRenderable::UsedRenderMethod()
{
	return mRenderMethod;
}

bool MeshRenderable::UsedDepth()
{
	return mUseDepth;
}


void MeshRenderable::SetRenderMethod(RenderMethod method)
{
	mRenderMethod = method;
}

void MeshRenderable::SetDepthUsage(bool active)
{
	mUseDepth = active;
}

void MeshRenderable::Init(RenderMethod useMethod, bool useDepth)
{
	mUseDepth = useDepth;
	mRenderMethod = useMethod;
}


void MeshRenderable::Draw()
{
	if (mUseDepth)
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	glPolygonMode(GL_FRONT_AND_BACK, mRenderMethod);
	if (mRenderMethod == RenderMethod::TRIANGLE)
	{
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (mRenderMethod == RenderMethod::P)
	{
		glPointSize(3.0f);
	}
	if (mRenderMethod == RenderMethod::LINE)
	{
		glLineWidth(1.0f);
	}

	mpMesh->Draw();
}

void MeshRenderable::Destroy()
{

}
