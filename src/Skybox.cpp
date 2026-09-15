#include "Skybox.h"

Skybox::Skybox()
{

}

Skybox::~Skybox()
{
	Destroy();
}

RenderProgram* Skybox::ActivateRenderProgram()
{
	mSkyboxProgram.SetActive();
	mSkyboxProgram.SetUniformMat4x4(
		mInvViewProj,
		"u_InvViewProj"
	);
	mSkyboxProgram.SetUniformVec3(
		mSkyColor,
		"u_TopColor"
	);
	mSkyboxProgram.SetUniformVec3(
		mGroundColor,
		"u_BottomColor"
	);
	return &mSkyboxProgram;
}


void Skybox::Init(RenderMethod useMethod, bool useDepth)
{
	glGenVertexArrays(1, &mVAO);

	mSkyboxProgram.Init(
		"res/skybox.vert",
		"res/skybox.frag"
	);
	mSkyboxProgram.UseDepth(false);
	mSkyboxProgram.RegisterUniform("u_BottomColor");
	mSkyboxProgram.RegisterUniform("u_TopColor");
	mSkyboxProgram.RegisterUniform("u_InvViewProj");
}

void Skybox::Draw()
{
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Skybox::Destroy()
{
	glDeleteVertexArrays(1, &mVAO);
}
