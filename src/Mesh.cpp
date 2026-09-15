#include "Mesh.h"
#include "Skybox.h"

Mesh::~Mesh()
{
	delete[] mpVertices;
}

void Mesh::Init()
{
    if (mpVertices == nullptr) return;
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // Initial data
    glBufferData(
        GL_ARRAY_BUFFER, 
        mVerticesCount * sizeof(vertex), 
        mpVertices, 
        mUsage
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);                      // Vec3f Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));    // Vec3f Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(6 * sizeof(float)));    // Vec2f UV
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::UpdateVertices()
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        mVerticesCount * sizeof(vertex),
        mpVertices,
        mUsage
    );
}

void Mesh::Draw()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, mVerticesCount);
}

void Mesh::Destroy()
{
    if (mpVertices == nullptr) return;
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void EditableHorizontalPlane::UpdateQuad(unsigned int x, unsigned int y, glm::vec2* buffer)
{
    glm::vec3 normal(0, 1, 0);

    float TopLeft = buffer[x + y * mWidth].x;
    float TopRight = buffer[(x + 1) + y * mWidth].x;
    float BottomLeft = (buffer[x + (y + 1) * mWidth].x);
    float BottomRight = buffer[(x + 1) + (y + 1) * mWidth].x;

    // TOP LEFT
    mpVertices[mVerticesCount++] = {
        { x, TopLeft, y },
        normal,
        { float(x) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // BOTTOM LEFT
    mpVertices[mVerticesCount++] = {
        { x, BottomLeft, y + 1 },
        normal,
        { float(x) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };

    // TOP RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, TopRight, y },
        normal,
        { float(x + 1) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // TOP RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, TopRight, y },
        normal,
        { float(x + 1) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // BOTTOM LEFT
    mpVertices[mVerticesCount++] = {
        { x, BottomLeft, y + 1 },
        normal,
        { float(x) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };

    // BOTTOM RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, BottomRight, y + 1 },
        normal,
        { float(x + 1) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };
}

void EditableHorizontalPlane::AddQuad(unsigned int x, unsigned int y, float h)
{
    glm::vec3 normal(0, 1, 0);

    // TOP LEFT
    mpVertices[mVerticesCount++] = {
        { x, h, y },
        normal,
        { float(x) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // BOTTOM LEFT
    mpVertices[mVerticesCount++] = {
        { x, h, y + 1 },
        normal,
        { float(x) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };

    // TOP RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, h, y },
        normal,
        { float(x + 1) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // TOP RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, h, y },
        normal,
        { float(x + 1) / (mWidth - 1), float(y) / (mHeight - 1) }
    };

    // BOTTOM LEFT
    mpVertices[mVerticesCount++] = {
        { x, h, y + 1 },
        normal,
        { float(x) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };

    // BOTTOM RIGHT
    mpVertices[mVerticesCount++] = {
        { x + 1, h, y + 1 },
        normal,
        { float(x + 1) / (mWidth - 1), float(y + 1) / (mHeight - 1) }
    };
}

void EditableHorizontalPlane::AddNeighbourX(unsigned int x, unsigned int y, float h, float h_other)
{
    if (h == h_other || x >= mWidth - 1)
        return;

    float xPos = float(x + 1);
    float z0 = float(y);
    float z1 = float(y + 1);

    auto uv0 = glm::vec2(
        xPos / float(mWidth - 1), 
        z0 / float(mHeight - 1)
    );
    auto uv1 = glm::vec2(
        xPos / float(mWidth - 1),
        z1 / float(mHeight - 1)
    );

    glm::vec3 normal = (h_other > h)
        ? glm::vec3(1, 0, 0)
        : glm::vec3(-1, 0, 0);

    // triangle 1
    mpVertices[mVerticesCount++] = { { xPos, h,        z0 }, normal, uv0 };
    mpVertices[mVerticesCount++] = { { xPos, h,        z1 }, normal, uv1 };
    mpVertices[mVerticesCount++] = { { xPos, h_other,  z0 }, normal, uv0 };

    // triangle 2
    mpVertices[mVerticesCount++] = { { xPos, h_other,  z0 }, normal, uv0 };
    mpVertices[mVerticesCount++] = { { xPos, h,        z1 }, normal, uv1 };
    mpVertices[mVerticesCount++] = { { xPos, h_other,  z1 }, normal, uv1 };
}

void EditableHorizontalPlane::AddNeighbourY(unsigned int x, unsigned int y, float h, float h_other)
{
    if (h == h_other || y >= mHeight - 1)
        return;

    float zPos = float(y + 1);
    float x0 = float(x);
    float x1 = float(x + 1);

    auto uv0 = glm::vec2(
        x0 / float(mHeight - 1),
        zPos / float(mWidth - 1)
    );
    auto uv1 = glm::vec2(
        x1 / float(mHeight - 1),
        zPos / float(mWidth - 1)
    );

    glm::vec3 normal = (h_other > h)
        ? glm::vec3(0, 0, 1)
        : glm::vec3(0, 0, -1);

    mpVertices[mVerticesCount++] = { { x0, h,        zPos }, normal, uv0 };
    mpVertices[mVerticesCount++] = { { x0, h_other,  zPos }, normal, uv0 };
    mpVertices[mVerticesCount++] = { { x1, h,        zPos }, normal, uv1 };

    mpVertices[mVerticesCount++] = { { x1, h,        zPos }, normal, uv1 };
    mpVertices[mVerticesCount++] = { { x0, h_other,  zPos }, normal, uv0 };
    mpVertices[mVerticesCount++] = { { x1, h_other,  zPos }, normal, uv1 };
}

EditableHorizontalPlane::EditableHorizontalPlane(unsigned int width, unsigned int height) :
    Mesh(GL_DYNAMIC_DRAW),
    mWidth(width),
    mHeight(height)
{
    unsigned int vertex_quad_count = (mWidth - 1) * (mHeight - 1) * 6;
    mVerticesCount = 0;
    mpVertices = new vertex[vertex_quad_count];

    //for (unsigned int x = 0; x < mWidth - 1; ++x)
    //{
    //    for (unsigned int y = 0; y < mHeight - 1; ++y)
    //    {
    //        //AddQuad(x, y, 0);
    //        //AddNeighbourX(x, y, 0, 0);
    //        //AddNeighbourY(x, y, 0, 0);
    //    }
    //}
}

void EditableHorizontalPlane::Update(glm::vec2* buffer)
{
    mVerticesCount = 0;
    for (unsigned int x = 0; x < mWidth - 1; ++x)
    {
        for (unsigned int y = 0; y < mHeight - 1; ++y)
        {
            UpdateQuad(x, y, buffer);

            //float current = buffer[x + y * mWidth].x;

            //float other_X = buffer[(x + 1) + y * mWidth].x;
            //float other_Y = buffer[x + (y + 1) * mWidth].x;

            //AddQuad(x, y, current);
            //AddNeighbourX(x, y, current, other_X);
            //AddNeighbourY(x, y, current, other_Y);
        }
    }

    UpdateVertices();
}


