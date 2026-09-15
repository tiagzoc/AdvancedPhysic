#include "Shader.h"
#include "Mesh.h"

Shader::Shader() : 
    mIsDestroyed(true),
    mShader(0)
{

}

Shader::~Shader()
{
    Destroy();
}

void Shader::Init(const char* path, ShaderType type)
{
    mType = type;
    char* ShaderSourceCode;
    std::ifstream file_vert(path, std::ios::ate | std::ios::binary);
    if (file_vert.is_open() == false)
    {
        std::cerr << "Failed to open shader file : " << path << std::endl;
        return;
    }

    std::streamsize size = file_vert.tellg();
    file_vert.seekg(0);
    ShaderSourceCode = new char[size + 1];
    ShaderSourceCode[size] = '\0';
    file_vert.read(ShaderSourceCode, size);

    //std::cout << "Shader Code Successfully Loaded : " << std::endl;
    //std::cout << ShaderSourceCode << std::endl;

    mShader = glCreateShader(type);
    glShaderSource(mShader, 1, &ShaderSourceCode, nullptr);
    glCompileShader(mShader);

    int success;
    glGetShaderiv(mShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(mShader, 512, nullptr, infoLog);
        std::cerr << infoLog << std::endl;
    }
    else {

        std::cout << "Successfully Compiled : " << path << std::endl;
        std::cout << ShaderSourceCode << std::endl;

    }
    delete[] ShaderSourceCode;
    mIsDestroyed = false;
}


void Shader::Destroy()
{
    if (mIsDestroyed) return;

    glDeleteShader(mShader);
    mShader = 0;

    mIsDestroyed = true;
}

Program::Program() :
    mIsDestroyed(true),
    mProgram(0),
    mUseDepth(true)
{

}

Program::~Program()
{
    Destroy();
}

void Program::InitWithShaders(Shader* pShaders, unsigned int shaderCount)
{
    mProgram = glCreateProgram();
    for (uint64_t i = 0; i < shaderCount; ++i)
    {
        glAttachShader(mProgram, pShaders[i].GetShader());
    }
    glLinkProgram(mProgram);
    mIsDestroyed = false;
}

void Program::Destroy()
{
    if (mIsDestroyed) return;

    glDeleteProgram(mProgram);
    mProgram = 0;

    mIsDestroyed = true;
}

void Program::SetActive()
{
    if (mUseDepth) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else {
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    glUseProgram(mProgram);
}

void Program::RegisterUniform(std::string u_ID)
{
    GLint uniform = glGetUniformLocation(mProgram, u_ID.c_str());
    if (uniform == -1)
    {
        std::cout << "uniform " << u_ID << " hasn't been found (he may have been removed due to optimization if not used)" << std::endl;
    }
    mUniform.emplace(u_ID, uniform);
}

GLint Program::GetUniform(std::string u_ID)
{
    auto it = mUniform.find(u_ID);
    if (it == mUniform.end())
    {
        return -1;
    }
    return (*it).second;
}

void Program::SetUniformMat4x4(const glm::mat4x4& mat, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniformMatrix4fv(
        uniform,
        1,
        GL_FALSE,
        glm::value_ptr(mat)
    );
}

void Program::SetUniformVec4(const glm::vec4& v, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniform4f(
        uniform,
        v.x,
        v.y,
        v.z,
        v.w
    );
}

void Program::SetUniformVec3(const glm::vec3& v, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniform3f(
        uniform,
        v.x,
        v.y,
        v.z
    );
}

void Program::SetUniformVec2(const glm::vec2& v, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniform2f(
        uniform,
        v.x,
        v.y
    );
}

void Program::SetUniformFloat32(float f, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniform1f(
        uniform,
        f
    );
}

void Program::SetUniformUInt32(uint32_t u, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;

    glUniform1ui(
        uniform,
        u
    );
}

void Program::SetUniformTexture(unsigned int index, Texture* pTex, std::string u_ID)
{
    GLint uniform = GetUniform(u_ID);
    if (uniform == -1) return;
    if (index >= 32) return;

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, pTex->Get());

    glUniform1i(uniform, index);
}

void RenderProgram::Init(const char* vertex_path, const char* fragment_path)
{
    Shader shaders[2];

    shaders[0].Init(vertex_path, ShaderType::VERTEX);
    shaders[1].Init(fragment_path, ShaderType::FRAGMENT);

    InitWithShaders(shaders, 2);
}

void ComputeProgram::Init(const char* compute_path)
{
    Shader shader;

    shader.Init(compute_path, ShaderType::COMPUTE);

    InitWithShaders(&shader, 1);
}
