#pragma once

#include "framework.h"

#include "AssetManager.h"
#include "Entity.h"

#include "Mesh.h"
#include "ComputeShader.h"

template<unsigned int WIDTH, unsigned int HEIGHT> requires(WIDTH % 2 == 1 && HEIGHT % 2 == 1)
struct ConvolutionGrid
{
	float GRID[WIDTH][HEIGHT];
	float normalizeFactor;

	constexpr ConvolutionGrid(const float(&factors)[WIDTH][HEIGHT])
		: GRID{}, normalizeFactor(0.0f)
	{
		float sum = 0.0f;
		for (unsigned int i = 0; i < WIDTH; ++i)
		{
			for (unsigned int j = 0; j < HEIGHT; ++j)
			{
				GRID[i][j] = factors[i][j];
				sum += factors[i][j];
			}
		}
		normalizeFactor = 1.0f / sum;
	}
};

static constexpr ConvolutionGrid<3, 3> Gaussian3x3()
{
	return ConvolutionGrid<3, 3>
	(
		{
			{ glm::exp(-glm::sqrt(2.0f)),				glm::exp(-1.0f),		glm::exp(-glm::sqrt(2.0f))	},
			{ glm::exp(-1.0f),							1,						glm::exp(-1.0f)				},
			{ glm::exp(-glm::sqrt(2.0f)),				glm::exp(-1.0f),		glm::exp(-glm::sqrt(2.0f))	}
		}
	);
}

static constexpr ConvolutionGrid<5, 5> Wave5x5()
{
	return ConvolutionGrid<5, 5>
		(
			{
				{ 
					exp(-sqrt(2.0f) * 1.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-0.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-sqrt(2.0f) * 1.5f)
				},
				
				{
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-sqrt(2.0f) * 0.5f),
					exp(-0.5f),
					exp(-sqrt(2.0f) * 0.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f)
				},

				{
					exp(-0.5f),
					exp(-0.5f),
					exp(-1.5f),
					exp(-0.5f),
					exp(-0.5f),
				},

				{
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-sqrt(2.0f) * 0.5f),
					exp(-0.5f),
					exp(-sqrt(2.0f) * 0.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f)
				},

				{
					exp(-sqrt(2.0f) * 1.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-0.5f),
					exp(-0.5f - sqrt(3.0f) * 0.5f),
					exp(-sqrt(2.0f) * 1.5f)
				}
			}
		);
}

static constexpr ConvolutionGrid<5, 5> Flat5x5()
{
	return ConvolutionGrid<5, 5>(
		{
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }
		}
	);
}

struct Water
{
	float height;
	float velocity;
};

class WaterManager
{
	unsigned int mWidth, mHeight;
	glm::vec2 mScale;
	Water* mpSurfaceDataCurrent;

	EditableHorizontalPlane* mpPlane;
	MeshRenderable* mpRenderable;
	MeshRenderable* mpWireframe;

	RenderProgram mWaterProgram;

	ComputeProgram mWaterSimulationProgram;
	ComputeShader mWaterSimulationCompute;

	ReadBuffer1D<Water>* mWaterInputGPU;
	WriteBuffer1D<Water>* mWaterOutputGPU;

	unsigned int GetPointCount() const { return mWidth * mHeight; }

	void RenderSurface(UpdateCtx* pCtx);

public:
	Water& PointSrc(unsigned int x, unsigned int y) { return mpSurfaceDataCurrent[x + y * mWidth]; }

	WaterManager();
	~WaterManager();

	void Init(unsigned int width, unsigned int height, float scalarX, float scalarY);

	void DropBox(unsigned int x, unsigned int y, float w, float h, float force);
	void BusterCall(unsigned int x, unsigned int y, float radius);
	void DropSphere(unsigned int x, unsigned int y, float radius);

	glm::ivec2 ToLocal(glm::vec3 point);


	float GetHeight(unsigned int x, unsigned int y);

	void AddForce(unsigned int x, unsigned int y, float force);
	void AddForceRadius(unsigned int x, unsigned int y, float radius, float force);
	void SetHeight(unsigned int x, unsigned int y, float height);

	void StepWater(UpdateCtx* pCtx);

	void Update(UpdateCtx* pCtx);
	void Render(UpdateCtx* pCtx);

};

