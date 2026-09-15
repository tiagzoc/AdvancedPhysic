#pragma once

#include "framework.h"
#include "Texture.h"

using Seconds = float;

enum ShaderType : int
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER,
	COMPUTE = GL_COMPUTE_SHADER
};

class Shader 
{
	GLuint mShader;
	ShaderType mType;
	bool mIsDestroyed;
public:
	Shader();
	virtual ~Shader();

	void Init(const char* path, ShaderType type);

	GLuint GetShader() const { return mShader; }
	ShaderType GetType() const { return mType; }

	void Destroy();
};

class Program
{
	bool mIsDestroyed;

	GLuint mProgram;
	bool mUseDepth;
	std::map<std::string, GLint> mUniform;
protected:
	Program();
	virtual ~Program();
	void InitWithShaders(Shader* pShaders, unsigned int shaderCount);
public:
	void SetActive();

	void UseDepth(bool active = true) { mUseDepth = active; }
	void RegisterUniform(std::string u_ID);
	GLint GetUniform(std::string u_ID);

	void SetUniformMat4x4(const glm::mat4x4& mat, std::string u_ID);
	void SetUniformVec4(const glm::vec4& v, std::string u_ID);
	void SetUniformVec3(const glm::vec3& v, std::string u_ID);
	void SetUniformVec2(const glm::vec2& v, std::string u_ID);
	void SetUniformFloat32(float f, std::string u_ID);
	void SetUniformUInt32(uint32_t u, std::string u_ID);
	void SetUniformTexture(unsigned int index, Texture* pTex, std::string u_ID);

	GLuint GetProgam() const { return mProgram; }
	void Destroy();
};

class RenderProgram : public Program
{
public:
	RenderProgram() : Program() {}
	virtual ~RenderProgram() {}
	void Init(const char* vertex_path, const char* fragment_path);
};

class ComputeProgram : public Program
{
public:
	ComputeProgram() : Program() {}
	virtual ~ComputeProgram() {}
	void Init(const char* compute_path);
};
