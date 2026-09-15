#pragma once

#include "framework.h"

#include <cmath>
#include <vector>

struct vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};


class Mesh
{
	GLuint mVAO, mVBO;
	bool mIsDestroyed = true;
protected:
	vertex* mpVertices = nullptr;
	uint64_t mVerticesCount = 0;
	GLenum mUsage;
	constexpr Mesh(GLenum usage) : mUsage(usage), mVAO(0), mVBO(0) {}
public:
	virtual ~Mesh();

	virtual void Init();

	virtual void UpdateVertices();

	virtual void Draw();

	const vertex* Vertices() const { return mpVertices; }
	uint64_t VerticesCount() const { return mVerticesCount; }

	void Destroy();
};

class CUBE : public Mesh
{
public:
	constexpr CUBE() : Mesh(GL_STATIC_DRAW) {
		mpVertices = new vertex[]{
			// FACE
			{{  1.0f,  1.0f, -1.0f}, { 0, 0, -1 }, {1, 1}},   // 0
			{{  1.0f, -1.0f, -1.0f}, { 0, 0, -1 }, {1 ,0}},   // 1
			{{ -1.0f, -1.0f, -1.0f}, { 0, 0, -1 }, {0, 0}},   // 2

			{{  1.0f,  1.0f, -1.0f}, { 0, 0, -1 }, {1, 1}},   // 0
			{{ -1.0f, -1.0f, -1.0f}, { 0, 0, -1 }, {0, 0}},   // 2
			{{ -1.0f,  1.0f, -1.0f}, { 0, 0, -1 }, {0, 1}},   // 3

			// BACK
			{{ -1.0f, -1.0f,  1.0f}, { 0, 0, 1 }, {0, 1}},    // 4
			{{  1.0f, -1.0f,  1.0f}, { 0, 0, 1 }, {1, 1}},    // 5
			{{  1.0f,  1.0f,  1.0f}, { 0, 0, 1 }, {1, 0}},    // 6

			{{ -1.0f, -1.0f,  1.0f}, { 0, 0, 1 }, {0, 1}},    // 4
			{{  1.0f,  1.0f,  1.0f}, { 0, 0, 1 }, {1, 0}},    // 6
			{{ -1.0f,  1.0f,  1.0f}, { 0, 0, 1 }, {0, 0}},    // 7

			// TOP
			{{ -1.0f,  1.0f, -1.0f}, { 0, 1, 0 }, {0, 1}},    // 3
			{{ -1.0f,  1.0f,  1.0f}, { 0, 1, 0 }, {0, 0}},    // 7
			{{  1.0f,  1.0f,  1.0f}, { 0, 1, 0 }, {1, 0}},    // 6

			{{ -1.0f,  1.0f, -1.0f}, { 0, 1, 0 }, {0, 1}},    // 3
			{{  1.0f,  1.0f,  1.0f}, { 0, 1, 0 }, {1, 0}},    // 6
			{{  1.0f,  1.0f, -1.0f}, { 0, 1, 0 }, {1, 1}},	   // 0

			// BOTTOM
			{{ -1.0f, -1.0f, -1.0f}, { 0, -1, 0 }, {0, 0}},   // 2
			{{  1.0f, -1.0f, -1.0f}, { 0, -1, 0 }, {1 ,0}},   // 1
			{{  1.0f, -1.0f,  1.0f}, { 0, -1, 0 }, {1, 1}},   // 5

			{{ -1.0f, -1.0f, -1.0f}, { 0, -1, 0 }, {0, 0}},   // 2
			{{  1.0f, -1.0f,  1.0f}, { 0, -1, 0 }, {1, 1}},   // 5
			{{ -1.0f, -1.0f,  1.0f}, { 0, -1, 0 }, {0, 1}},   // 4

			// LEFT
			{{ -1.0f,  1.0f, -1.0f}, { -1, 0, 0 }, {1, 0}},   // 11
			{{ -1.0f, -1.0f, -1.0f}, { -1, 0, 0 }, {1, 1}},   // 10
			{{ -1.0f, -1.0f,  1.0f}, { -1, 0, 0 }, {0, 1}},   // 4

			{{ -1.0f,  1.0f, -1.0f}, { -1, 0, 0 }, {1, 0}},   // 11
			{{ -1.0f, -1.0f,  1.0f}, { -1, 0, 0 }, {0, 1}},   // 4
			{{ -1.0f,  1.0f,  1.0f}, { -1, 0, 0 }, {0, 0}},   // 7

			// RIGHT
			{{  1.0f,  1.0f, -1.0f}, { 1, 0, 0 }, {0, 0}},    // 8
			{{  1.0f,  1.0f,  1.0f}, { 1, 0, 0 }, {1, 0}},    // 6
			{{  1.0f, -1.0f,  1.0f}, { 1, 0, 0 }, {1, 1}},    // 5

			{{  1.0f,  1.0f, -1.0f}, { 1, 0, 0 }, {0, 0}},    // 8
			{{  1.0f, -1.0f,  1.0f}, { 1, 0, 0 }, {1, 1}},    // 5
			{{  1.0f, -1.0f, -1.0f}, { 1, 0, 0 }, {0, 1}},    // 9
		};
		mVerticesCount = 36;
	}
};


class SPHERE : public Mesh
{
public:
	constexpr SPHERE() : Mesh(GL_STATIC_DRAW) {
		constexpr float phi_steps = 2 * glm::pi<float>() / 8; // 8 steps arround the sphere
		constexpr float teta_steps = glm::pi<float>() / 7; // 6 stage to the sphere + 2 poles
		constexpr unsigned int triangle_count = 2 * 8 * 6; // 2 triangle / iteration  X 8 iteration arround X 6 stages
		constexpr float INV_PI = 1 / glm::pi<float>();
		constexpr float INV_PI2 = 1 / (glm::pi<float>() * 2.0f);

		mVerticesCount = triangle_count * 3;
		mpVertices = new vertex[mVerticesCount];

		uint64_t i_vertex_current = 0;
		
		for (unsigned int it_stage = 1; it_stage < 7; ++it_stage) // 1 - 6
		{
			float offset = (it_stage % 2 == 0 ? phi_steps * 0.5f : 0);
			for (unsigned int it_arround = 0; it_arround < 8; ++it_arround) // 0 - 7
			{
				float phi_current = phi_steps * it_arround;
				float phi_next = phi_steps * (it_arround + 1);
				float phi_top_bottom = phi_steps * (it_arround + 0.5f);

				float teta_current_next = teta_steps * it_stage;
				float teta_top = teta_steps * (it_stage - 1);
				float teta_bottom = teta_steps * (it_stage + 1);

				float normalized_phi_current = phi_current * INV_PI2;
				float normalized_phi_next = phi_next * INV_PI2;
				float normalized_phi_top_bottom = phi_top_bottom * INV_PI2;

				float normalized_teta_current_next = teta_current_next * INV_PI;
				float normalized_teta_top = teta_top * INV_PI;
				float normalized_teta_bottom = teta_bottom * INV_PI;


				teta_current_next -= glm::pi<float>() * 0.5f;
				teta_top -= glm::pi<float>() * 0.5f;
				teta_bottom -= glm::pi<float>() * 0.5f;

				phi_current += offset;
				phi_next += offset;
				phi_top_bottom += offset;

				float cos_phi_current = cos(phi_current);
				float sin_phi_current = sin(phi_current);
				float cos_phi_next = cos(phi_next);
				float sin_phi_next = sin(phi_next);
				float cos_phi_top_bottom = cos(phi_top_bottom);
				float sin_phi_top_bottom = sin(phi_top_bottom);

				float cos_teta_current_next = cos(teta_current_next);
				float sin_teta_current_next = sin(teta_current_next);
				float cos_teta_top = cos(teta_top);
				float sin_teta_top = sin(teta_top);
				float cos_teta_bottom = cos(teta_bottom);
				float sin_teta_bottom = sin(teta_bottom);

				glm::vec3 current = { cos_phi_current * cos_teta_current_next, sin_teta_current_next, sin_phi_current * cos_teta_current_next };
				glm::vec3 next = { cos_phi_next * cos_teta_current_next, sin_teta_current_next, sin_phi_next * cos_teta_current_next };
				glm::vec3 top = { cos_phi_top_bottom * cos_teta_top, sin_teta_top, sin_phi_top_bottom * cos_teta_top };
				glm::vec3 bottom = { cos_phi_top_bottom * cos_teta_bottom, sin_teta_bottom, sin_phi_top_bottom * cos_teta_bottom };

				glm::vec3 one = glm::vec3{ 1.0f, 1.0f, 1.0f };

				mpVertices[i_vertex_current].position = top;
				mpVertices[i_vertex_current].normal = top;
				mpVertices[i_vertex_current].uv = 0.5f * (one - top);
				i_vertex_current++;
				mpVertices[i_vertex_current].position = current;
				mpVertices[i_vertex_current].normal = current;
				mpVertices[i_vertex_current].uv = 0.5f * (one - current);
				i_vertex_current++;
				mpVertices[i_vertex_current].position = next;
				mpVertices[i_vertex_current].normal = next;
				mpVertices[i_vertex_current].uv = 0.5f * (one - next);
				i_vertex_current++;
				mpVertices[i_vertex_current].position = current;
				mpVertices[i_vertex_current].normal = current;
				mpVertices[i_vertex_current].uv = 0.5f * (one - current);
				i_vertex_current++;
				mpVertices[i_vertex_current].position = bottom;
				mpVertices[i_vertex_current].normal = bottom;
				mpVertices[i_vertex_current].uv = 0.5f * (one - bottom);
				i_vertex_current++;
				mpVertices[i_vertex_current].position = next;
				mpVertices[i_vertex_current].normal = next;
				mpVertices[i_vertex_current].uv = 0.5f * (one - next);
				i_vertex_current++;


			}
		}

	}
};

class EditableHorizontalPlane : public Mesh
{
	const unsigned int mWidth, mHeight;

	void UpdateQuad_Height(unsigned int x, unsigned int y, glm::vec2* buffer);
	void UpdateQuad_Neighbour(unsigned int x, unsigned int y, glm::vec2* buffer);

	void UpdateQuad(unsigned int x, unsigned int y, glm::vec2* buffer);

	void AddQuad(unsigned int x, unsigned int y, float h);
	void AddNeighbourX(unsigned int x, unsigned int y, float h, float h_other);
	void AddNeighbourY(unsigned int x, unsigned int y, float h, float h_other);
public:
	EditableHorizontalPlane(unsigned int width, unsigned int height);
	
	void Update(glm::vec2* buffer);

};
