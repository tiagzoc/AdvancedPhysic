#pragma once

#include "framework.h"

#include "Mesh.h"
#include "Shader.h"

struct AssetManager
{

private:
	inline static RenderProgram Unlit;
	inline static RenderProgram Wireframe;
	inline static CUBE* pCube;
	inline static SPHERE* pSphere;
	inline static Texture Obama;
	inline static Texture TE;
	inline static Texture EAU;

	inline static Texture* mpHeads;

public:

	static void Init();

	static void Destroy();

	static RenderProgram* GetUnlitProgram() { return &Unlit; }
	static RenderProgram* GetWireframeProgram() { return &Wireframe; }
	static CUBE* GetCube() { return pCube; }
	static SPHERE* GetSphere() { return pSphere; }
	static Texture* GetObama() { return &Obama; }
	static Texture* GetTE() { return &TE; }
	static Texture* GetEAU() { return &EAU; }
	static Texture* GetHead(unsigned int i) { return &mpHeads[i]; }
	static unsigned int GetHeadCount() { return 17; }
};

