#include "WaterManager.h"

void WaterManager::RenderSurface(UpdateCtx* pCtx)
{
	float W = static_cast<float>(mWidth) * mScale.x;
	float H = static_cast<float>(mHeight) * mScale.y;

	glm::mat4 T = Translate({ -W * 0.5f, 0, -H * 0.5f });
	glm::mat4 S = Scale({ mScale.x, 1, mScale.y });
	glm::mat4 world = T * S;

	auto pProgram = mpRenderable->ActivateRenderProgram();
	pProgram->SetUniformMat4x4(world, "u_World");
	pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
	pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
	pProgram->SetUniformMat4x4(*pCtx->pVP * world, "u_WVP");
	pProgram->SetUniformTexture(0, AssetManager::GetEAU(), "u_Texture0");
	mpRenderable->Draw();

	//glm::mat4 World2 = Translate({ -static_cast<float>(mWidth) * 0.5f, -1.0f, -static_cast<float>(mHeight) * 0.5f });
	//auto pWireframe = mpWireframe->ActivateRenderProgram();
	//pWireframe->SetUniformMat4x4(World2, "u_World");
	//pWireframe->SetUniformMat4x4(*pCtx->pView, "u_View");
	//pWireframe->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
	//pWireframe->SetUniformMat4x4(*pCtx->pVP * World2, "u_WVP");
	//pWireframe->SetUniformFloat32(1000, "u_MaxDistance");
	//pWireframe->SetUniformVec3(glm::vec3{ 1, 0, 0 }, "u_WireFrameColor");
	//mpWireframe->Draw();
}






WaterManager::WaterManager() :
	mWaterProgram(),
	mWaterSimulationProgram()
{
	mWaterProgram.Init("res/water.vert", "res/water.frag");
	mWaterProgram.RegisterUniform("u_World");
	mWaterProgram.RegisterUniform("u_View");
	mWaterProgram.RegisterUniform("u_Proj");
	mWaterProgram.RegisterUniform("u_WVP");

	mWaterSimulationProgram.Init("res/WaterSim.comp");
	mWaterSimulationProgram.RegisterUniform("u_numWater");
	mWaterSimulationProgram.RegisterUniform("u_Surface");
	mWaterSimulationProgram.RegisterUniform("u_WaveSpeed");
	mWaterSimulationProgram.RegisterUniform("u_PosDumping");
	mWaterSimulationProgram.RegisterUniform("u_VelDumping");
	mWaterSimulationProgram.RegisterUniform("u_DeltaTime");
	mWaterSimulationProgram.RegisterUniform("u_Width");
	mWaterSimulationProgram.RegisterUniform("u_Height");



}

WaterManager::~WaterManager()
{
	delete mpPlane;
	delete mpRenderable;
	delete mpWireframe;
}

void WaterManager::Init(unsigned int width, unsigned int height, float scalarX, float scalarY)
{

	mWidth = width;
	mHeight = height;
	mScale = { scalarX, scalarY };

	mpSurfaceDataCurrent = new Water[GetPointCount()];
	for (int x = 0; x < mWidth; ++x)
	{
		for (int y = 0; y < mWidth; ++y)
		{
			int index = x + y * mWidth;
			mpSurfaceDataCurrent[index].height = 0;
			mpSurfaceDataCurrent[index].velocity = 0;
		}
	}

	mWaterSimulationCompute.Init(&mWaterSimulationProgram, { 32, 32, 1 }, { mWidth, mHeight, 1 });

	mWaterInputGPU = mWaterSimulationCompute.CreateReadBuffer<Water>();
	mWaterInputGPU->binding = 0;
	mWaterInputGPU->element_count = mWidth * mHeight;
	mWaterInputGPU->pData = mpSurfaceDataCurrent;
	mWaterInputGPU->Init();

	mWaterOutputGPU = mWaterSimulationCompute.CreateWriteBuffer<Water>();
	mWaterOutputGPU->binding = 1;
	mWaterOutputGPU->element_count = mWidth * mHeight;
	mWaterOutputGPU->Init();

	mpPlane = new EditableHorizontalPlane(mWidth, mHeight);
	mpPlane->Init();

	mpRenderable = new MeshRenderable(&mWaterProgram, mpPlane);
	mpRenderable->Init(RenderMethod::TRIANGLE);

	mpWireframe = new MeshRenderable(AssetManager::GetWireframeProgram(), mpPlane);
	mpWireframe->Init(RenderMethod::LINE, false);
}

void WaterManager::DropBox(unsigned int x, unsigned int y, float w, float h, float force)
{
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			float dx = i - w * 0.5f;
			float dy = j - h * 0.5f;
			unsigned int _x = x + dx;
			if (_x < 0 || _x >= mWidth) continue;
			unsigned int _y = y + dy;
			if (_y < 0 || _y >= mHeight) continue;

				
			PointSrc(
				_x, 
				_y
			).height -= force;
		}
	}
}

void WaterManager::BusterCall(unsigned int x, unsigned int y, float radius)
{
	float volume_substacted = 0;
	for (float i = 0; i < radius * 2; i++)
	{
		for (float j = 0; j < radius * 2; j++)
		{
			float dx = i - radius;
			float dy = j - radius;

			unsigned int _x = x + dx;
			if (_x < 0 || _x >= mWidth) continue;
			unsigned int _y = y + dy;
			if (_y < 0 || _y >= mHeight) continue;

			float sq_distance = dx * dx + dy * dy;
			if (sq_distance > radius * radius) continue;

			float force = radius - sqrt(sq_distance);

			PointSrc(
				_x,
				_y
			).height -= force;
			volume_substacted += force;
		}
	}
	unsigned int precision = 100;
	float volume_per_addition = volume_substacted / precision;

	for (float angle = 0; angle < glm::pi<float>() * 2; angle += (glm::pi<float>() * 2.0f / float(precision)))
	{
		glm::vec2 pos = radius * glm::vec2{ cos(angle), sin(angle) } + glm::vec2{ x, y };
		if (pos.x < 0 || pos.x >= mWidth) continue;
		if (pos.y < 0 || pos.y >= mHeight) continue;
		PointSrc(
			pos.x,
			pos.y
		).height += volume_per_addition;
	}
}

void WaterManager::DropSphere(unsigned int x, unsigned int y, float radius)
{
	float totalVolume = 0.0f;
	float invSigma2 = 1 / 36.0f;
	float strength = 0.15f;

	for (int dy = -radius; dy <= radius; ++dy)
		for (int dx = -radius; dx <= radius; ++dx)
		{
			unsigned int _x = x + dx;
			if (_x < 0 || _x >= mWidth) continue;
			unsigned int _y = y + dy;
			if (_y < 0 || _y >= mHeight) continue;

			float sq_distance = dx * dx + dy * dy;
			if (sq_distance > radius * radius) continue;

			float delta = radius - sqrt(sq_distance);

			PointSrc(
				_x,
				_y
			).height -= delta;
			/*
				float r2 = dx * dx + dy * dy;
				if (r2 > radius * radius) continue;

				float k = exp(-r2 * invSigma2);
				float delta = k * strength;

				unsigned int _x = x + dx;
				if (_x < 0 || _x >= mWidth) continue;
				unsigned int _y = y + dy;
				if (_y < 0 || _y >= mHeight) continue;

				PointSrc(_x,_y).height -= delta;
			*/
			totalVolume += delta;
		}


	float ringVolume = 0.0f;
	int R2 = radius * 1.5f;

	// first pass: compute normalization
	for (int dy = -R2; dy <= R2; ++dy)
		for (int dx = -R2; dx <= R2; ++dx)
		{
			float r = sqrt(dx * dx + dy * dy);
			if (r < radius || r > R2) continue;

			float k = exp(-(r - radius) * (r - radius) * invSigma2);
			ringVolume += k;
		}

	// second pass: add volume
	for (int dy = -R2; dy <= R2; ++dy)
		for (int dx = -R2; dx <= R2; ++dx)
		{
			float r = sqrt(dx * dx + dy * dy);
			if (r < radius || r > R2) continue;

			float k = exp(-(r - radius) * (r - radius) * invSigma2);

			unsigned int _x = x + dx;
			if (_x < 0 || _x >= mWidth) continue;
			unsigned int _y = y + dy;
			if (_y < 0 || _y >= mHeight) continue;

			PointSrc(_x, _y).height += totalVolume * (k / ringVolume);
		}
}

glm::ivec2 WaterManager::ToLocal(glm::vec3 point)
{
	return glm::uvec2{
		(point.x / mScale.x) + mWidth * 0.5f,
		(point.z / mScale.y) + mHeight * 0.5f,
	};
}

float WaterManager::GetHeight(unsigned int x, unsigned int y)
{
	return PointSrc(x, y).height;
}

void WaterManager::AddForce(unsigned int x, unsigned int y, float force)
{
	PointSrc(x, y).velocity += force;
}

void WaterManager::AddForceRadius(unsigned int x, unsigned int y, float radius, float force)
{
	float radiusX = radius / mScale.x;
	float radiusY = radius / mScale.y;

	for (int dy = -radiusY; dy <= radiusY; ++dy)
	{		
		for (int dx = -radiusX; dx <= radiusX; ++dx)
		{
			unsigned int _x = x + dx;
			if (_x < 0 || _x >= mWidth) continue;
			unsigned int _y = y + dy;
			if (_y < 0 || _y >= mHeight) continue;

			float r = dx * dx + dy * dy;
			if (r > radiusX * radiusY) continue;

			PointSrc(_x, _y).velocity += force;
		}
	}
}

void WaterManager::SetHeight(unsigned int x, unsigned int y, float height)
{
	PointSrc(x, y).height = height;
}

void WaterManager::StepWater(UpdateCtx* pCtx)
{
	unsigned int numWater = mWidth * mHeight;
	float surface = 1.0f;
	float wave_speed = glm::min(5.0f, 0.5f / float(pCtx->deltaTime));
	float pos_dumping = glm::min(1.0f, 1.0f * float(pCtx->deltaTime));
	float vel_dumping = glm::max(0.0f, 1.0f - 0.3f * float(pCtx->deltaTime));
	float dt = pCtx->deltaTime;

	//std::cout << "dt : " << dt << std::endl;
	//std::cout << "wave_speed : " << wave_speed << std::endl;
	//std::cout << "pos_dumping : " << pos_dumping << std::endl;
	//std::cout << "vel_dumping : " << vel_dumping << std::endl;

	auto pProgram = mWaterSimulationCompute.ActivateProgram();
	mWaterInputGPU->Update(mpSurfaceDataCurrent);
	mWaterSimulationCompute.SetElementCount({ mWidth, mHeight, 1});
	pProgram->SetUniformUInt32(numWater, "u_numWater");
	pProgram->SetUniformFloat32(surface, "u_Surface");
	pProgram->SetUniformFloat32(wave_speed, "u_WaveSpeed");
	pProgram->SetUniformFloat32(pos_dumping, "u_PosDumping");
	pProgram->SetUniformFloat32(vel_dumping, "u_VelDumping");
	pProgram->SetUniformFloat32(dt, "u_DeltaTime");
	pProgram->SetUniformUInt32(mWidth, "u_Width");
	pProgram->SetUniformUInt32(mHeight, "u_Height");

	mWaterSimulationCompute.Compute();

	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	Water* water = (Water*)mWaterOutputGPU->StartRead();

	memcpy(mpSurfaceDataCurrent, water, mWidth * mHeight * sizeof(Water));
	
	mWaterOutputGPU->EndRead();


	//float pos_dumping = ;
	//float velocity_dumping = ;

	//for (unsigned int x = 0; x < mWidth; ++x)
	//{
	//	for (unsigned int y = 0; y < mHeight; ++y)
	//	{
	//		float h_Left = PointSrc(x, y).height;
	//		float h_Right = PointSrc(x, y).height;
	//		float h_Top = PointSrc(x, y).height;
	//		float h_Bottom = PointSrc(x, y).height;

	//		if (x > 0) h_Left = PointSrc(x - 1, y).height;
	//		if (y > 0) h_Right = PointSrc(x, y - 1).height;
	//		if (x < mWidth - 1) h_Top = PointSrc(x + 1, y).height;
	//		if (y < mHeight - 1) h_Bottom = PointSrc(x, y + 1).height;

	//		float sum_h = h_Left + h_Right + h_Top + h_Bottom;

	//		PointDest(x, y).velocity = PointSrc(x, y).velocity + (sum_h - 4 * PointSrc(x, y).height) * pCtx->deltaTime * C2 / S2;
	//		PointDest(x, y).height = PointSrc(x, y).height + (0.25f * sum_h - PointSrc(x, y).height) * pos_dumping;

	//		PointDest(x, y).velocity *= velocity_dumping;
	//		PointDest(x, y).height = PointDest(x, y).height + PointDest(x, y).velocity * pCtx->deltaTime;
	//	}
	//}

	//Water* temp = mpSurfaceDataCurrent;
	//mpSurfaceDataCurrent = mpSurfaceDataNext;
	//mpSurfaceDataNext = temp;
}


void WaterManager::Update(UpdateCtx* pCtx)
{
	mpPlane->Update((glm::vec2*)mpSurfaceDataCurrent);	
}


void WaterManager::Render(UpdateCtx* pCtx)
{
	RenderSurface(pCtx);
}