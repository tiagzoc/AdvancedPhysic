#include "PhysicManager.h"

void PhysicManager::UpdateImpacts(float dt)
{
    unsigned int* pAtomic = (unsigned int*)mpAtomic->StartRead();

    unsigned int written = (*pAtomic < MAX_IMPACTS ? *pAtomic : MAX_IMPACTS);

    mpAtomic->EndRead();

    if (written == 0) return;

    Impact_GPU* pOutput = (Impact_GPU*)mpImpact_GPU->StartRead();

    //std::cout << "Impact Count : " << written << std::endl;

    mUpdateImpacts.UnsafeSetResource(pOutput, written);
    unsigned int impact_by_thread = written / (mUpdateImpacts.GetThreadCount() - 1);
    for (int i = 0; i < mUpdateImpacts.GetThreadCount() - 1; ++i)
    {
        auto delimiter = (TMM::ThreadListDelimiter1D*)mUpdateImpacts.UnsafeDelimiter(i);
    
        delimiter->start = impact_by_thread * i;
        delimiter->offset = written;
        delimiter->stride = impact_by_thread;
        delimiter->end = written;
    }

    unsigned int remaining = written - impact_by_thread * (mUpdateImpacts.GetThreadCount() - 1);
    auto delimiter = (TMM::ThreadListDelimiter1D*)mUpdateImpacts.UnsafeDelimiter(mUpdateImpacts.GetThreadCount() - 1);
    delimiter->start = impact_by_thread * (mUpdateImpacts.GetThreadCount() - 1);
    delimiter->offset = written;
    delimiter->stride = remaining;
    delimiter->end = written;

    mUpdateImpacts.ProcessAllLayers();
    mUpdateImpacts.WaitForAllEnd();

    mpImpact_GPU->EndRead();

    //while (mImpactQueue.size() > 0)
    //{
    //    Impact& impact = mImpactQueue.front();

    //    ResolveImpact(impact);

    //    mImpactQueue.pop();
    //}
}

glm::uvec3 PhysicManager::PositionToCellCoord(glm::vec3 pos)
{
    glm::vec3 v = glm::vec3{ 0.5f, 0.5f, 0.5f } + (pos / (mpBoundingBox->GetScale() * 2.0f));
    return {
        v.x * mSpacePartitioningCount.x,
        v.y * mSpacePartitioningCount.y,
        v.z * mSpacePartitioningCount.z
    };
}

void PhysicManager::UpdateRigidBody(Entity* pEntity, float dt)
{
    if (pEntity->GetRigidBody() != nullptr)
    {
        pEntity->GetRigidBody()->Update(dt);
        CheckForInBoundingBox(pEntity);
    }

}

void PhysicManager::CheckForInBoundingBox(Entity* pEntity)
{
    Transform& transform = pEntity->GetTransform();
    glm::vec3 bounding_box = pEntity->GetBoundingBox(); // TODO : take into acount for rotation
    glm::vec3 pos = transform.GetPosition();

    if (pos.x - bounding_box.x < -mpBoundingBox->GetScale().x)
    {
        transform.SetX(
            -mpBoundingBox->GetScale().x + bounding_box.x
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ 1, 0, 0 });
    }
    else if (pos.x + bounding_box.x > mpBoundingBox->GetScale().x)
    {
        transform.SetX(
            mpBoundingBox->GetScale().x - bounding_box.x
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ -1, 0, 0 });
    }

    if (pos.y - bounding_box.y < -mpBoundingBox->GetScale().y)
    {
        transform.SetY(
            -mpBoundingBox->GetScale().y + bounding_box.y
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ 0, 1, 0 });
    }
    else if (pos.y + bounding_box.y > mpBoundingBox->GetScale().y)
    {
        transform.SetY(
            mpBoundingBox->GetScale().y - bounding_box.y
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ 0, -1, 0 });
    }

    if (pos.z - bounding_box.z < -mpBoundingBox->GetScale().z)
    {
        transform.SetZ(
            -mpBoundingBox->GetScale().z + bounding_box.z
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ 0, 0, 1 });
    }
    else if (pos.z + bounding_box.z > mpBoundingBox->GetScale().z)
    {
        transform.SetZ(
            mpBoundingBox->GetScale().z - bounding_box.z
        );
        pEntity->GetRigidBody()->BounceBoundingBox({ 0, 0, -1 });
    }
}

//void PhysicManager::CheckForImpact(Entity* pEntity1, Entity* pEntity2)
//{
//    Impact impact(pEntity1, pEntity2);
//    if (CollisionCheck::Collide(pEntity1, pEntity2, impact))
//        mImpactQueue.push(impact);
//}

void PhysicManager::ResetCollider()
{
    for (auto pEntity : mpEntities)
    {
        pEntity->Is_Colliding = false;
    }

    mUpdateColliders.UnsafeSetResource(mColliders.data(), mColliders.size());
    unsigned int impact_by_thread = mColliders.size() / (mUpdateImpacts.GetThreadCount() - 1);
    for (int i = 0; i < mUpdateColliders.GetThreadCount() - 1; ++i)
    {
        auto delimiter = (TMM::ThreadListDelimiter1D*)mUpdateColliders.UnsafeDelimiter(i);

        delimiter->start = impact_by_thread * i;
        delimiter->offset = mColliders.size();
        delimiter->stride = impact_by_thread;
        delimiter->end = mColliders.size();
    }

    unsigned int remaining = mColliders.size() - impact_by_thread * (mUpdateColliders.GetThreadCount() - 1);
    auto delimiter = (TMM::ThreadListDelimiter1D*)mUpdateColliders.UnsafeDelimiter(mUpdateColliders.GetThreadCount() - 1);
    delimiter->start = impact_by_thread * (mUpdateColliders.GetThreadCount() - 1);
    delimiter->offset = mColliders.size();
    delimiter->stride = remaining;
    delimiter->end = mColliders.size();

    mUpdateColliders.ProcessAllLayers();
    mUpdateColliders.WaitForAllEnd();
}

PhysicManager::PhysicManager() :
    mShowMeshs(true),
    mShowWireframes(false),
    mShowPointeds(false),
    mShowSpacePartitioning(false),
    mSpacePatritioningWireframe(AssetManager::GetWireframeProgram(), AssetManager::GetCube())
{

}

PhysicManager::~PhysicManager()
{
    for (auto pEntity : mpEntities)
    {
        delete pEntity;
    }
}

void PhysicManager::Init(Transform* pBoundingBox, glm::uvec3 space_partitioning)
{
    mpBoundingBox = pBoundingBox;
    mSpacePartitioningCount = space_partitioning;
    mSpacePartitioningSize = {
        mpBoundingBox->GetScale().x * 2.0f / static_cast<float>(space_partitioning.x),
        mpBoundingBox->GetScale().y * 2.0f / static_cast<float>(space_partitioning.y),
        mpBoundingBox->GetScale().z * 2.0f / static_cast<float>(space_partitioning.z)
    };
    mSpacePatritioningWireframe.Init(RenderMethod::LINE, false);

    mPhysicComputeProgram.Init("res/PhysicCompute.comp");
    mPhysicComputeProgram.RegisterUniform("numColliders");
    mPhysicComputeProgram.RegisterUniform("maxImpacts");

    mPhysicComputeShader.Init(&mPhysicComputeProgram, { 256, 1, 1 }, {0, 0, 0});
    mpCollider_GPU = mPhysicComputeShader.CreateReadBuffer<Collider_GPU>();
    mpImpact_GPU = mPhysicComputeShader.CreateWriteBuffer<Impact_GPU>();
    mpAtomic = mPhysicComputeShader.CreateAtomicBuffer<uint32_t>();
    
    mpCollider_GPU->pData = nullptr;
    mpCollider_GPU->element_count = MAX_ENTITIES;
    mpCollider_GPU->binding = 0;
    mpCollider_GPU->Init();

    mpImpact_GPU->element_count = MAX_IMPACTS;
    mpImpact_GPU->binding = 1;
    mpImpact_GPU->Init();

    mpAtomic->default_value = 0;
    mpAtomic->binding = 2;
    mpAtomic->Init();

    TMM::ThreadListDescriptor<Impact_GPU> desc{};
    desc.pFunc = TMM::MakeMethodPtr(this, &PhysicManager::ResolveImpact);;
    desc.threadCount = TMM::Thread::GetMaxThreadCount();
    desc.resource.pData = nullptr;
    desc.resource.dataSize = 0;
    desc.pThreadDelimiters = new TMM::ThreadListDelimiter * [desc.threadCount];
    for (int i = 0; i < desc.threadCount; ++i)
    {
        desc.pThreadDelimiters[i] = new TMM::ThreadListDelimiter1D(0, 1, 1, 1);
    }
    mUpdateImpacts.Init(&desc);
    mUpdateImpacts.Start();

    TMM::ThreadListDescriptor<Collider_GPU> desc2{};
    desc2.pFunc = TMM::MakeMethodPtr(this, &PhysicManager::UpdateCollider);;
    desc2.threadCount = TMM::Thread::GetMaxThreadCount();
    desc2.resource.pData = nullptr;
    desc2.resource.dataSize = 0;
    desc2.pThreadDelimiters = new TMM::ThreadListDelimiter * [desc2.threadCount];
    for (int i = 0; i < desc2.threadCount; ++i)
    {
        desc2.pThreadDelimiters[i] = new TMM::ThreadListDelimiter1D(0, 1, 1, 1);
    }
    mUpdateColliders.Init(&desc2);
    mUpdateColliders.Start();
}

Entity* PhysicManager::AddEntity(Shape shape, bool addCollider, bool addRigidBody)
{
    Mesh* pMesh;
    switch (shape)
    {
    case Shape::CUBE_AABB: pMesh = AssetManager::GetCube(); break;
    case Shape::SPHERE: pMesh = AssetManager::GetSphere(); break;
    default: pMesh = AssetManager::GetCube(); break;
    }

	Entity* pEntity = new Entity(AssetManager::GetWireframeProgram(), AssetManager::GetUnlitProgram(), pMesh); 
    pEntity->SetShowMesh(mShowMeshs);
    pEntity->SetShowWireCollider(mShowWireframes);
    pEntity->SetShowPointed(mShowPointeds);
    pEntity->SetMaxRenderDistanceDebug(20.0f);
    mColliders.push_back({});

    if(addCollider)
        pEntity->CreateCollider(shape);
    if(addRigidBody)
        pEntity->CreateRigidBody();

	mpEntities.push_back(pEntity);

	return pEntity;
}

void PhysicManager::ToggleMeshVisibility()
{
    mShowMeshs = !mShowMeshs;
    for (auto pEntity : mpEntities)
    {
        pEntity->SetShowMesh(mShowMeshs);
    }
}

void PhysicManager::ToggleWireframeVisibility()
{
    mShowWireframes = !mShowWireframes;
    for (auto pEntity : mpEntities)
    {
        pEntity->SetShowWireCollider(mShowWireframes);
    }
}

void PhysicManager::TogglePointsVisibility()
{
    mShowPointeds = !mShowPointeds;
    for (auto pEntity : mpEntities)
    {
        pEntity->SetShowPointed(mShowPointeds);
    }
}

void PhysicManager::ToggleSpacePartitioningVisibility()
{
    mShowSpacePartitioning = !mShowSpacePartitioning;
}

void PhysicManager::ResolveImpact(Impact_GPU& edited_data, uint64_t index_edited, const Impact_GPU* unsafe_resource, uint64_t resouce_size)
{
    RigidBody* pRigidBody1 = mpEntities[edited_data.entity_index1]->GetRigidBody();
    RigidBody* pRigidBody2 = mpEntities[edited_data.entity_index2]->GetRigidBody();

    float mass1 = 0;
    float mass2 = 0;
    float mass1_ratio = 0;
    float mass2_ratio = 0;

    if (pRigidBody1 != nullptr)
        mass1 = pRigidBody1->GetMass();
    if (pRigidBody2 != nullptr)
        mass2 = pRigidBody2->GetMass();

    if (pRigidBody1 == nullptr)
    {
        mass1_ratio = 1;
    }
    else if (pRigidBody2 == nullptr)
    {
        mass2_ratio = 1;
    }
    else {
        float inv_mass_sum = 1.0f / (mass1 + mass2);
        mass1_ratio = mass2 * inv_mass_sum;
        mass2_ratio = mass1 * inv_mass_sum;
    }

    if (pRigidBody1 != nullptr)
    {
        glm::vec3 N = { 0, 0, 0 }; // - impact.normal * pRigidBody1->GetMass() * pRigidBody1->Gravity;
        glm::vec3 Ff = -pRigidBody1->GetVelocity() * pRigidBody1->Coef_Friction;

        pRigidBody1->AddForce(N + Ff);
        pRigidBody1->AddVelocityBounce(-edited_data.normal * mass1_ratio);
        pRigidBody1->AddOffset(-edited_data.normal * edited_data.penetration);
    }
    if (pRigidBody2 != nullptr)
    {
        glm::vec3 N = { 0, 0, 0 }; // = impact.normal * pRigidBody2->GetMass() * pRigidBody2->Gravity;
        glm::vec3 Ff = -pRigidBody2->GetVelocity() * pRigidBody2->Coef_Friction;

        pRigidBody2->AddForce(N + Ff);
        pRigidBody2->AddVelocityBounce(edited_data.normal * mass2_ratio);
        pRigidBody2->AddOffset(edited_data.normal * edited_data.penetration);
    }

    mpEntities[edited_data.entity_index1]->Is_Colliding = true;
    mpEntities[edited_data.entity_index2]->Is_Colliding = true;
}

void PhysicManager::UpdateCollider(Collider_GPU& edited_data, uint64_t index_edited, const Collider_GPU* unsafe_resource, uint64_t resouce_size)
{
   edited_data.entity_index   = index_edited;
   edited_data.position       = glm::vec4(mpEntities[index_edited]->GetTransform().GetPosition(), 0.0f);
   edited_data.half_extent    = glm::vec4(mpEntities[index_edited]->GetTransform().GetScale(), 0.0f);
   edited_data.rotation       = mpEntities[index_edited]->GetTransform().GetMatrixRotation();
   edited_data.type           = (uint32_t)mpEntities[index_edited]->GetCollider()->Type;
   edited_data.cell_coords    = glm::uvec4(PositionToCellCoord(mpEntities[index_edited]->GetTransform().GetPosition()), 1.0f);
}

void PhysicManager::UpdateCollision()
{
    ResetCollider();

    auto pProgram = mPhysicComputeShader.ActivateProgram();
    mpAtomic->Update();
    mpCollider_GPU->Update(mColliders.data(), mColliders.size());
    mPhysicComputeShader.SetElementCount({ mColliders.size(), 1, 1});
    pProgram->SetUniformUInt32(mColliders.size(), "numColliders");
    pProgram->SetUniformUInt32(MAX_IMPACTS, "maxImpacts");
    mPhysicComputeShader.Compute();
}

void PhysicManager::Update(UpdateCtx* pCtx)
{
    if (mpEntities.size() == 0) return;

    float sub_dt = pCtx->deltaTime / pCtx->physic_step_count;
    for (int i = 0; i < pCtx->physic_step_count; ++i)
    {
        UpdateCollision();
        UpdateImpacts(sub_dt);
        for (auto pEntity : mpEntities)
        {
            UpdateRigidBody(pEntity, sub_dt);
        }
    }
}

void PhysicManager::Render(UpdateCtx* pCtx)
{
    if (mShowMeshs)
    {
        for (auto pEntity : mpEntities)
        {
            pEntity->Render(pCtx);
        }
    }

    if (mShowWireframes)
    {
        for (auto pEntity : mpEntities)
        {
            pEntity->RenderWireCollider(pCtx);
        }
    }

    if (mShowPointeds)
    {
        for (auto pEntity : mpEntities)
        {
            pEntity->RenderPointedMesh(pCtx);
        }
    }


    if (mShowSpacePartitioning)
    {
        Transform transform;
        transform.SetScale(mSpacePartitioningSize * 0.5f);

        for (int x = 0; x < mSpacePartitioningCount.x; ++x)
        {
            for (int y = 0; y < mSpacePartitioningCount.y; ++y)
            {
                for (int z = 0; z < mSpacePartitioningCount.z; ++z)
                {
                    transform.SetPosition({
                        mSpacePartitioningSize.x* x - (mSpacePartitioningSize.x * (mSpacePartitioningCount.x - 1) * 0.5f),
                        mSpacePartitioningSize.y* y - (mSpacePartitioningSize.y * (mSpacePartitioningCount.y - 1) * 0.5f),
                        mSpacePartitioningSize.z* z - (mSpacePartitioningSize.z * (mSpacePartitioningCount.z - 1) * 0.5f)
                    });

                    auto pProgram = mSpacePatritioningWireframe.ActivateRenderProgram();
                    pProgram->SetUniformMat4x4(transform.GetMatrix(), "u_World");
                    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
                    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
                    pProgram->SetUniformMat4x4(*pCtx->pVP * transform.GetMatrix(), "u_WVP");
                    pProgram->SetUniformVec3(glm::vec3{ 0, 0, 1 }, "u_WireFrameColor");
                    pProgram->SetUniformFloat32(75, "u_MaxDistance");
                    mSpacePatritioningWireframe.Draw();
                }
            }
        }
    }
}


void PhysicManager::Destroy()
{
	for (auto pEntity : mpEntities)
	{
		delete pEntity;
	}
}