#pragma once

#include "framework.h"

#include "PhysicManager.h"
#include "WaterManager.h"

#include "Skybox.h"
#include "Camera.h"
#include "InputManager.h"

#include <chrono>

#define PROFILE_PHYSIC_STEP_COUNT_HIGH 10 
#define PROFILE_PHYSIC_STEP_COUNT_MID 5 
#define PROFILE_PHYSIC_STEP_COUNT_LOW 3 

#define PROFILE_PHYSIC_FIXED_DT_HIGH (1.0f / 120.0f)
#define PROFILE_PHYSIC_FIXED_DT_MID (1.0f / 90.0f)
#define PROFILE_PHYSIC_FIXED_DT_LOW (1.0f / 60.0f)

#define DEFAULT_PHYSIC_STEP_COUNT PROFILE_PHYSIC_STEP_COUNT_MID
#define DEFAULT_PHYSIC_FIXED_DT PROFILE_PHYSIC_FIXED_DT_HIGH

struct SceneDesc
{
	float Gravity					= RigidBody::DEFAULT_GRAVITY;
	float Air_Viscosity				= RigidBody::DEFAULT_AIR_VISCOSITY;
	float Coef_Friction				= RigidBody::DEFAULT_COEF_FRICTION;
	float Coef_Friction_BoundingBox	= RigidBody::DEFAULT_COEF_FRICTION_BOUNDING_BOX;
	float FixedDeltaTime			= DEFAULT_PHYSIC_FIXED_DT;
	float CameraSpeed				= 5;
	unsigned int PhysicStepCount	= DEFAULT_PHYSIC_STEP_COUNT;
	glm::uvec3 Space_Partitioning	= { 10, 10, 10 };
	glm::vec3 ExperimentBoxSize = { 90.0f, 90.0f, 90.0f };
};

class Scene
{
	GLFWwindow* mpWindow;

	Skybox mSkyBox;
	Camera mCamera;
	
	float mLocalTime						= 0;
	float mLocalTimeSinceLastFixedUpdate	= 0;
	float mDt								= 0;
protected:
	SceneDesc mDesc;

	virtual void HandleInputs(UpdateCtx* pCtx);
	virtual void Update(UpdateCtx* pCtx);
	virtual void Render(UpdateCtx* pCtx);

	// Inherited class must have a constructor like : HeritedScene(GLFWwindow* pWindow)
	Scene(GLFWwindow* pWindow, const SceneDesc& desc);
public:

	virtual void Init();

	virtual void Frame();
	void Activate();

	Camera& GetCamera() { return mCamera; }
};


class ScenePhysic : public Scene
{
	PhysicManager mManager;
	MeshRenderable mBoundingBox;
	Transform mBoundingBoxTransform;
protected:
	ScenePhysic(GLFWwindow* pWindow, glm::vec3 experiment_size, glm::uvec3 space_partition, const SceneDesc& desc);

	virtual void HandleInputs(UpdateCtx* pCtx) override;
	virtual void Update(UpdateCtx* pCtx) override;
	virtual void Render(UpdateCtx* pCtx) override;

public:
	PhysicManager& GetManager() { return mManager; }
};

class SceneWater : public Scene
{
	WaterManager mManager;
	glm::uvec2 mSize;
	float timer = 0;
	bool mActivateRain;
	
public:
	SceneWater(GLFWwindow* pWindow);

	virtual void HandleInputs(UpdateCtx* pCtx) override;
	virtual void Update(UpdateCtx* pCtx) override;
	virtual void Render(UpdateCtx* pCtx) override;
};

class SceneAll : public Scene
{
protected:
	PhysicManager mPhysicManager;
	MeshRenderable mBoundingBox;
	Transform mBoundingBoxTransform;

	WaterManager mWaterManager;
	glm::uvec2 mSize;

public:

	SceneAll(GLFWwindow* pWindow, glm::vec3 experiment_size, glm::uvec3 space_partition, const SceneDesc& desc);

	virtual void HandleInputs(UpdateCtx* pCtx) override;
	virtual void Update(UpdateCtx* pCtx) override;
	virtual void Render(UpdateCtx* pCtx) override;
};

class SceneAll_Implement : public SceneAll
{

public:
	SceneAll_Implement(GLFWwindow* pWindow) : 
		SceneAll(
			pWindow,
			glm::vec3{ 90.0f, 90.0f, 90.0f },
			glm::uvec3{ 10, 10, 10 },
			SceneDesc{
				.Gravity = RigidBody::DEFAULT_GRAVITY,
				.FixedDeltaTime = PROFILE_PHYSIC_FIXED_DT_LOW,
				.PhysicStepCount = PROFILE_PHYSIC_STEP_COUNT_MID
			}
		)
	{

	}

	void Init() override {
		
		for (int i = 0; i < 10; ++i)
		{
			float mass = float(rand() % 1000) / 1000.f * 4 + 1;
			auto entity_massive = mPhysicManager.AddEntity(Shape::SPHERE);
			entity_massive->GetRigidBody()->SetMass(mass);
			entity_massive->GetTransform().SetScale({ mass, mass, mass });
			entity_massive->GetTransform().SetPosition({
				float(rand() % 1000) / 1000.f * 80 - 40,
				float(rand() % 1000) / 1000.f * 35 + 5,
				float(rand() % 1000) / 1000.0f * 80 - 40
			});
		}
	}
};


template<
	Shape CONTROL,
	Shape STATIC,
	bool GRAVITY_ON = true
>
class ContolableShape : public ScenePhysic
{
	Entity* mpControl;
	Entity* mpStatic;
public:
	ContolableShape(GLFWwindow* pWindow) :
		ScenePhysic(
			pWindow, 
			glm::vec3{ 90.0f, 90.0f, 90.0f },
			glm::uvec3{ 10, 10, 10 },
			SceneDesc{
				.Gravity = (GRAVITY_ON ? RigidBody::DEFAULT_GRAVITY : 0),
				.FixedDeltaTime = PROFILE_PHYSIC_FIXED_DT_LOW,
				.PhysicStepCount = PROFILE_PHYSIC_STEP_COUNT_MID
			}
		)
	{ }

	void Init() override {

		mpControl = GetManager().AddEntity(CONTROL);
		mpControl->GetTransform().SetRotation({ 1, 1, 0 });
		mpControl->GetTransform().SetPosition({ 3, 0, -1 });
			

		mpStatic = GetManager().AddEntity(STATIC);
	}
	void Update(UpdateCtx* pCtx) override {
		ScenePhysic::Update(pCtx);
		if(InputManager::KeyDown(GLFW_KEY_UP))
			mpControl->GetTransform().AddPosition({ 0, 0, pCtx->deltaTime * 10 });

		if (InputManager::KeyDown(GLFW_KEY_DOWN))
			mpControl->GetTransform().AddPosition({ 0, 0, -pCtx->deltaTime * 10 });

		if (InputManager::KeyDown(GLFW_KEY_LEFT))
			mpControl->GetTransform().AddPosition({ pCtx->deltaTime * 10, 0, 0 });

		if (InputManager::KeyDown(GLFW_KEY_RIGHT))
			mpControl->GetTransform().AddPosition({ -pCtx->deltaTime * 10, 0, 0 });


		//mpStatic->GetTransform().SetPosition({ 0, 0, 0 });

		//if (InputManager::KeyDown(GLFW_KEY_UP))
		//	mpControl->GetRigidBody()->AddVelocityBounce({ 0, 0, pCtx->deltaTime * 10 });

		//if (InputManager::KeyDown(GLFW_KEY_DOWN))
		//	mpControl->GetRigidBody()->AddVelocityBounce({ 0, 0, -pCtx->deltaTime * 10 });

		//if (InputManager::KeyDown(GLFW_KEY_LEFT))
		//	mpControl->GetRigidBody()->AddVelocityBounce({ pCtx->deltaTime * 10, 0, 0 });

		//if (InputManager::KeyDown(GLFW_KEY_RIGHT))
		//	mpControl->GetRigidBody()->AddVelocityBounce({ -pCtx->deltaTime * 10, 0, 0 });
	}
};

template<
	unsigned COUNT, 
	Shape SHAPE1, 
	bool GRAVITY_ON = true, 
	float MIN_SCALE = 1.0f,
	float MAX_SCALE = 1.0f,
	Shape SHAPE2 = SHAPE1,
	Shape SHAPE3 = SHAPE1,
	glm::vec3 BOUNDING_BOX = glm::vec3{ 90.0f, 90.0f, 90.0f },
	unsigned int STEPS = PROFILE_PHYSIC_STEP_COUNT_MID,
	glm::uvec3 PARTITION = glm::uvec3{ 10, 10, 10 }
>
class Pool : public ScenePhysic
{
public:
	Pool(GLFWwindow* pWindow) :
		ScenePhysic(
			pWindow,
			BOUNDING_BOX,
			PARTITION,
			SceneDesc{
				.Gravity = (GRAVITY_ON ? RigidBody::DEFAULT_GRAVITY : 0),
				.FixedDeltaTime = PROFILE_PHYSIC_FIXED_DT_MID,
				.PhysicStepCount = STEPS,
				.Space_Partitioning = PARTITION,
				.ExperimentBoxSize = BOUNDING_BOX

			}
		)
	{ };

	void Init() override {

		for (int i = 0; i < COUNT; ++i)
		{
			unsigned int shape = rand() % 3;
			Shape s = shape == 0 ? SHAPE1 : (shape == 1 ? SHAPE2 : SHAPE3);
			auto pEntity = GetManager().AddEntity(s);
			pEntity->GetTransform().SetPosition({
				((static_cast<float>(rand() % 1000) / 1000.0f) * BOUNDING_BOX.x - BOUNDING_BOX.x * 0.5f) * 0.9f,
				((static_cast<float>(rand() % 1000) / 1000.0f) * BOUNDING_BOX.y - BOUNDING_BOX.y * 0.5f) * 0.9f,
				((static_cast<float>(rand() % 1000) / 1000.0f) * BOUNDING_BOX.z - BOUNDING_BOX.z * 0.5f) * 0.9f
				});

			float scale = (static_cast<float>(rand() % 1000) / 1000.0f) * (MAX_SCALE - MIN_SCALE) + MIN_SCALE;

			pEntity->GetTransform().SetScale({ 
				scale,
				scale,
				scale
			});
			pEntity->GetRigidBody()->SetMass(scale);


			if (s == Shape::CUBE_OBB)
			{
				float r_angle1 = (static_cast<float>(rand() % 1000) / 1000.0f) * glm::pi<float>();
				float r_angle2 = (static_cast<float>(rand() % 1000) / 1000.0f) * glm::pi<float>();
				pEntity->GetTransform().SetRotation({ r_angle1, r_angle2, 0 });
			}
		}

	}
};

template< unsigned COUNT >
class AbusivePool : public Pool < COUNT, Shape::SPHERE, true, 1.0f, 2.0f, Shape::SPHERE, Shape::SPHERE, glm::vec3{ 180.0f, 180.0f, 180.0f }, PROFILE_PHYSIC_STEP_COUNT_LOW,  glm::uvec3{20, 20, 20} >
{
	using Parent = Pool<COUNT, Shape::SPHERE, true, 1.0f, 2.0f, Shape::SPHERE, Shape::SPHERE, glm::vec3{ 180.0f, 180.0f, 180.0f }, PROFILE_PHYSIC_STEP_COUNT_LOW, glm::uvec3{ 20, 20, 20 } >;
public:
	AbusivePool(GLFWwindow* pWindow) : Parent(pWindow) { }

	void Init() override
	{
		Parent::Init();

		this->GetCamera().transform.SetPosition({ 0, 1, -50 });
		this->GetCamera().transform.SetPitch(0.3f);
	}

	void HandleInputs(UpdateCtx* pCtx) override
	{
		InputManager::Update();

		// === Run ===
		bool SHIFT = false;
		if (InputManager::KeyDown(GLFW_KEY_LEFT_SHIFT)) SHIFT = true;

		// === Controls ===
		if (InputManager::KeyDown(GLFW_KEY_W))
		{
			this->GetCamera().transform.AddPosition(this->GetCamera().transform.GetForward() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}
		if (InputManager::KeyDown(GLFW_KEY_S))
		{
			this->GetCamera().transform.AddPosition(-this->GetCamera().transform.GetForward() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}
		if (InputManager::KeyDown(GLFW_KEY_A))
		{
			this->GetCamera().transform.AddPosition(this->GetCamera().transform.GetRight() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}
		if (InputManager::KeyDown(GLFW_KEY_D))
		{
			this->GetCamera().transform.AddPosition(-this->GetCamera().transform.GetRight() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}
		if (InputManager::KeyDown(GLFW_KEY_LEFT_CONTROL))
		{
			this->GetCamera().transform.AddPosition(-this->GetCamera().transform.GetUp() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}
		if (InputManager::KeyDown(GLFW_KEY_SPACE))
		{
			this->GetCamera().transform.AddPosition(this->GetCamera().transform.GetUp() * this->mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(pCtx->deltaTime));
		}

		// === Debug Visibility ===
		if (InputManager::KeyPressed(GLFW_KEY_1))
		{
			this->GetManager().ToggleMeshVisibility();
		}
		if (InputManager::KeyPressed(GLFW_KEY_2))
		{
			this->GetManager().ToggleWireframeVisibility();
		}
		if (InputManager::KeyPressed(GLFW_KEY_3))
		{
			this->GetManager().TogglePointsVisibility();
		}
		if (InputManager::KeyPressed(GLFW_KEY_4))
		{
			this->GetManager().ToggleSpacePartitioningVisibility();
		}
	}
	
};

template<
	unsigned COUNT,
	float MIN_SCALE = 1.0f,
	float MAX_SCALE = 1.0f,
	float PROJECTILE_SCALE = 5.0f,
	glm::vec3 BOUNDING_BOX = glm::vec3{ 90.0f, 90.0f, 90.0f }
>
class ShootingRange : public Pool < COUNT, Shape::SPHERE, false, MIN_SCALE, MAX_SCALE, Shape::SPHERE, Shape::SPHERE, BOUNDING_BOX>
{
public:
	ShootingRange(GLFWwindow* pWindow) : Pool<COUNT, Shape::SPHERE, false, MIN_SCALE, MAX_SCALE, Shape::SPHERE, Shape::SPHERE, BOUNDING_BOX>(pWindow) {}

	void Update(UpdateCtx* pCtx) override
	{
		ScenePhysic::Update(pCtx);
		if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{

			auto pEntity = this->GetManager().AddEntity(Shape::SPHERE);
			pEntity->GetTransform().SetPosition(this->GetCamera().transform.GetPosition());
			pEntity->GetTransform().SetScale({ PROJECTILE_SCALE, PROJECTILE_SCALE, PROJECTILE_SCALE });
			pEntity->GetRigidBody()->SetMass(PROJECTILE_SCALE);
			pEntity->GetRigidBody()->SetVelocity(this->GetCamera().transform.GetForward() * 500.0f);
			
		}
	}
};


