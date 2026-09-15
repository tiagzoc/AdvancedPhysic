#include "Scene.h"
#include "Scene.h"


void Scene::HandleInputs(UpdateCtx* pCtx)
{
    InputManager::Update();

    // === Camera Rotation ===
    InputManager::SetMousePosition({ WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f });
    float mouse_dx = mDt * 500 * (InputManager::MouseDeltaX() / WINDOW_WIDTH);
    float mouse_dy = mDt * 500 * (InputManager::MouseDeltaY() / WINDOW_HEIGHT);
    mCamera.transform.AddRotation({ -mouse_dx, mouse_dy, 0 });
    if (mCamera.transform.GetPitch() > glm::pi<float>() * 0.4f) mCamera.transform.SetPitch(glm::pi<float>() * 0.4f);
    if (mCamera.transform.GetPitch() < -glm::pi<float>() * 0.4f) mCamera.transform.SetPitch(-glm::pi<float>() * 0.4f);

    // === Run ===
    bool SHIFT = false;
    if (InputManager::KeyDown(GLFW_KEY_LEFT_SHIFT)) SHIFT = true;

    // === Controls ===
    if (InputManager::KeyDown(GLFW_KEY_W))
    {
        mCamera.transform.AddPosition(mCamera.transform.GetForward() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
    if (InputManager::KeyDown(GLFW_KEY_S))
    {
        mCamera.transform.AddPosition(-mCamera.transform.GetForward() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
    if (InputManager::KeyDown(GLFW_KEY_A))
    {
        mCamera.transform.AddPosition(mCamera.transform.GetRight() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
    if (InputManager::KeyDown(GLFW_KEY_D))
    {
        mCamera.transform.AddPosition(-mCamera.transform.GetRight() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
    if (InputManager::KeyDown(GLFW_KEY_LEFT_CONTROL))
    {
        mCamera.transform.AddPosition(-mCamera.transform.GetUp() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
    if (InputManager::KeyDown(GLFW_KEY_SPACE))
    {
        mCamera.transform.AddPosition(mCamera.transform.GetUp() * mDesc.CameraSpeed * (SHIFT ? 10.0f : 1.0f) * static_cast<float>(mDt));
    }
}


void Scene::Update(UpdateCtx* pCtx)
{
}


void Scene::Render(UpdateCtx* pCtx)
{

}

Scene::Scene(GLFWwindow* pWindow, const SceneDesc& desc)
{
    mDesc = desc;
    mpWindow = pWindow;

    mCamera.transform.SetPosition({ 0, 0, -50 });
    mSkyBox.Init(RenderMethod::TRIANGLE, false);
    mSkyBox.mSkyColor = { 0.1f, 0.4f, 1.0f };
    mSkyBox.mGroundColor = { 0.5f, 0.5f, 0.5f };

}

void Scene::Init()
{

}

void Scene::Activate()
{
    RigidBody::Gravity = mDesc.Gravity;
    RigidBody::Air_Viscosity = mDesc.Air_Viscosity;
    RigidBody::Coef_Friction = mDesc.Coef_Friction;
    RigidBody::Coef_Friction_Bounding_Box = mDesc.Coef_Friction_BoundingBox;
}

void Scene::Frame()
{
    auto start = std::chrono::high_resolution_clock::now();    

    glm::mat4 View = mCamera.GetView();
    glm::mat4 VP = mCamera.Proj * View;

    UpdateCtx ctx;
    ctx.pView = &View;
    ctx.pProj = &mCamera.Proj;
    ctx.pVP = &VP;
    ctx.deltaTime = mDt;
    ctx.time = mLocalTime;
    ctx.physic_step_count = mDesc.PhysicStepCount;

    HandleInputs(&ctx);

    float fixed_dt = mDesc.FixedDeltaTime;
    if (mDt > mDesc.FixedDeltaTime) fixed_dt = mDt;
    
    if (mLocalTimeSinceLastFixedUpdate > fixed_dt)
    {
        ctx.deltaTime = fixed_dt;
        Update(&ctx);
        mLocalTimeSinceLastFixedUpdate -= fixed_dt;
    }
    ctx.deltaTime = mDt;

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 ViewNoTranslate = glm::mat4((glm::mat3)*ctx.pView);
    glm::mat4 VP_Skybox = *ctx.pProj * ViewNoTranslate;
    mSkyBox.UpdateInvViewProj(glm::inverse(VP_Skybox));
    mSkyBox.ActivateRenderProgram();
    mSkyBox.Draw();

    Render(&ctx);

    glfwSwapBuffers(mpWindow);
    glfwPollEvents();

    auto end = std::chrono::high_resolution_clock::now();
    auto time_stamp = end - start;
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_stamp).count();
    mDt = static_cast<double>(ns) * 0.000000001;
    mLocalTimeSinceLastFixedUpdate += mDt;
    mLocalTime += mDt;
}

ScenePhysic::ScenePhysic(GLFWwindow* pWindow, glm::vec3 experiment_size, glm::uvec3 space_partition, const SceneDesc& desc) :
    Scene(pWindow, desc), 
    mBoundingBox(AssetManager::GetWireframeProgram(), AssetManager::GetCube())
{
    mBoundingBox.Init(RenderMethod::LINE, true);
    mBoundingBoxTransform.SetScale(mDesc.ExperimentBoxSize * 0.5f);
    mManager.Init(&mBoundingBoxTransform, mDesc.Space_Partitioning);
}

void ScenePhysic::HandleInputs(UpdateCtx* pCtx)
{
    Scene::HandleInputs(pCtx);

    // === Debug Visibility ===
    if (InputManager::KeyPressed(GLFW_KEY_1))
    {
        mManager.ToggleMeshVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_2))
    {
        mManager.ToggleWireframeVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_3))
    {
        mManager.TogglePointsVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_4))
    {
        mManager.ToggleSpacePartitioningVisibility();
    }
}

void ScenePhysic::Update(UpdateCtx* pCtx)
{
    Scene::Update(pCtx);
    mManager.Update(pCtx);
}

void ScenePhysic::Render(UpdateCtx* pCtx)
{
    Scene::Render(pCtx);

    mManager.Render(pCtx);

    glm::mat4& bounding_box_world = mBoundingBoxTransform.GetMatrix();
    auto pProgram = mBoundingBox.ActivateRenderProgram();
    pProgram->SetUniformMat4x4(bounding_box_world, "u_World");
    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
    pProgram->SetUniformMat4x4(*pCtx->pVP * bounding_box_world, "u_WVP");
    pProgram->SetUniformVec3({ 0, 1, 0 }, "u_WireFrameColor");
    pProgram->SetUniformFloat32(10000.0f, "u_MaxDistance");
    mBoundingBox.Draw();

}

SceneWater::SceneWater(GLFWwindow* pWindow) :
    Scene(
        pWindow,
        SceneDesc{
            .Gravity = RigidBody::DEFAULT_GRAVITY,
            .FixedDeltaTime = PROFILE_PHYSIC_FIXED_DT_LOW,
            .PhysicStepCount = PROFILE_PHYSIC_STEP_COUNT_MID
        }
    )
{
    mSize = { 500, 500 };
    mActivateRain = false;
    mManager.Init(mSize.x, mSize.y, 1, 1);

    GetCamera().transform.SetPosition({ 0, 150, -250 });
    GetCamera().transform.SetPitch(0.8f);
}

void SceneWater::HandleInputs(UpdateCtx* pCtx)
{
    Scene::HandleInputs(pCtx);
    if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        mManager.DropBox(rand() % mSize.x, rand() % mSize.y, 15, 15, 5.0f);
    }

    if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        mManager.DropSphere(rand() % mSize.x, rand() % mSize.y, 50.0f);
    }

    if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
    {
        mManager.BusterCall(rand() % mSize.x, rand() % mSize.y, 75.0f);
    }

    if (InputManager::KeyPressed(GLFW_KEY_M))
    {
        mManager.BusterCall(rand() % mSize.x, rand() % mSize.y, 150.0f);
    }

    if (InputManager::KeyPressed(GLFW_KEY_1))
    {
        mActivateRain = !mActivateRain;
    }

}


void SceneWater::Update(UpdateCtx * pCtx)
{
    Scene::Update(pCtx);
    mManager.Update(pCtx);
    mManager.StepWater(pCtx);

    if (mActivateRain)
    {
        constexpr float drop_per_sec = 1.0f / 100.0f;
        timer += pCtx->deltaTime;
        while (timer > drop_per_sec)
        {
            timer -= drop_per_sec;
            mManager.DropSphere(rand() % mSize.x, rand() % mSize.y, 4.0f * float(rand() % 1000) / 1000.0f + 2.0f);
        }
    }

}

void SceneWater::Render(UpdateCtx* pCtx)
{
    Scene::Render(pCtx);
    mManager.Render(pCtx);
}

SceneAll::SceneAll(GLFWwindow* pWindow, glm::vec3 experiment_size, glm::uvec3 space_partition, const SceneDesc& desc) :
    Scene(pWindow, desc),
    mBoundingBox(AssetManager::GetWireframeProgram(), AssetManager::GetCube())
{
    mBoundingBox.Init(RenderMethod::LINE, true);
    mBoundingBoxTransform.SetScale(mDesc.ExperimentBoxSize * 0.5f);
    mPhysicManager.Init(&mBoundingBoxTransform, mDesc.Space_Partitioning);


    mSize = { 100, 100 };
    mWaterManager.Init(mSize.x, mSize.y, experiment_size.x / float(mSize.x), experiment_size.y / float(mSize.y));
}


void SceneAll::HandleInputs(UpdateCtx* pCtx)
{
    Scene::HandleInputs(pCtx);

    // === Physic ===
    if (InputManager::KeyPressed(GLFW_KEY_1))
    {
        mPhysicManager.ToggleMeshVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_2))
    {
        mPhysicManager.ToggleWireframeVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_3))
    {
        mPhysicManager.TogglePointsVisibility();
    }
    if (InputManager::KeyPressed(GLFW_KEY_4))
    {
        mPhysicManager.ToggleSpacePartitioningVisibility();
    }

    // === Water ===
    if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        mWaterManager.DropBox(rand() % mSize.x, rand() % mSize.y, 15, 15, 5.0f);
    }
    if (InputManager::MouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        mWaterManager.DropSphere(rand() % mSize.x, rand() % mSize.y, 10.0f);
    }
}

void SceneAll::Update(UpdateCtx* pCtx)
{
    Scene::Update(pCtx);

    // === Physic ===
    mPhysicManager.Update(pCtx);

    // === Water ===
    mWaterManager.Update(pCtx);
    mWaterManager.StepWater(pCtx);

    for (unsigned int i = 0; i < mPhysicManager.GetEntityCount(); ++i)
    {
        Entity* pEntity = mPhysicManager.GetEntity(i);
        
        auto water_pos = mWaterManager.ToLocal(pEntity->GetTransform().GetPosition());

        float radius = pEntity->GetTransform().GetMaxScale();
        float water_height = mWaterManager.GetHeight(water_pos.x, water_pos.y);
        float player_height = pEntity->GetTransform().GetPosition().y;

        //std::cout << "Entity Level : " << player_height << " | Water Level : " << water_height << std::endl;


        if (water_height > player_height - radius)
        {

            float ratio = abs(player_height - water_height) / radius;
            float volume = 1.33333f * glm::pi<float>() * radius * radius * radius;
            float submerged = volume * ratio;

            float mass_volumique = pEntity->GetRigidBody()->GetMass() / volume;
      
            //std::cout << "MASS VOLUMIQUE : " << mass_volumique << std::endl;

            pEntity->GetRigidBody()->AddForce({ 0, RigidBody::Gravity * volume * (mass_volumique >= 2.0f ? 0.0f : pEntity->GetRigidBody()->GetMass()), 0 });

            glm::vec3 damping = -pEntity->GetRigidBody()->GetVelocity() * ratio * 5.0f;

            pEntity->GetRigidBody()->AddForce(damping);

            if (water_height < player_height + radius)
            {
                //mWaterManager.SetHeight(water_pos.x, water_pos.y, player_height);
                float force = glm::dot(pEntity->GetRigidBody()->GetVelocity(), { glm::vec3{0, 1, 0} });
                force = TMM_CLAMP(force, -5, 5);

                mWaterManager.AddForceRadius(water_pos.x, water_pos.y, radius, force);
            }
        }
    }

}

void SceneAll::Render(UpdateCtx* pCtx)
{
    Scene::Render(pCtx);

    // === Physic ===
    mPhysicManager.Render(pCtx);

    glm::mat4& bounding_box_world = mBoundingBoxTransform.GetMatrix();
    auto pProgram = mBoundingBox.ActivateRenderProgram();
    pProgram->SetUniformMat4x4(bounding_box_world, "u_World");
    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
    pProgram->SetUniformMat4x4(*pCtx->pVP * bounding_box_world, "u_WVP");
    pProgram->SetUniformVec3({ 0, 1, 0 }, "u_WireFrameColor");
    pProgram->SetUniformFloat32(10000.0f, "u_MaxDistance");
    mBoundingBox.Draw();

    // === Water ===
    mWaterManager.Render(pCtx);
}
