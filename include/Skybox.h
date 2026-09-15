#pragma once

#include "framework.h"
#include "Shader.h"
#include "Renderable.h"

class Skybox : public Renderable {
	GLuint mVAO;
	RenderProgram mSkyboxProgram;
	glm::mat4x4 mInvViewProj;

	virtual void UseMesh(Mesh* pMesh) { };

	virtual RenderMethod UsedRenderMethod() override { return RenderMethod::LINE; };
	virtual bool UsedDepth() override { return false; }

	virtual void SetRenderMethod(RenderMethod method) override { }
	virtual void SetDepthUsage(bool active) override { }

	virtual void UseRenderProgram(RenderProgram* pProgram) override { }
public:
	glm::vec3 mSkyColor;
	glm::vec3 mGroundColor;

	Skybox();
	virtual ~Skybox();

	virtual RenderProgram* ActivateRenderProgram() override;

	void UpdateInvViewProj(const glm::mat4x4& mat) { mInvViewProj = mat; }
	
	virtual void Init(RenderMethod useMethod = RenderMethod::LINE, bool useDepth = false) override;
	virtual void Draw() override;
	virtual void Destroy() override;
};