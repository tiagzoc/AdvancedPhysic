#include "Entity.h"


Entity::Entity(RenderProgram* pWireframe, RenderProgram* pUnlit, Mesh* pMesh) :
    mMesh(pUnlit, pMesh),
    mWireColliderMesh(pWireframe, pMesh),
    mPointedMesh(pWireframe, pMesh),
    mpCollider(nullptr),
    mpRigidBody(nullptr),
	mShowWireCollider(true),
    Is_Colliding(false)
{
	mMesh.Init();
    mWireColliderMesh.Init(RenderMethod::LINE, false);
    mPointedMesh.Init(RenderMethod::P, false);

    int i = rand() % (AssetManager::GetHeadCount() + 2);
    if (i == AssetManager::GetHeadCount()) mpTex = AssetManager::GetObama();
    else if (i == AssetManager::GetHeadCount() + 1) mpTex = AssetManager::GetTE();
    else mpTex = AssetManager::GetHead(i);
}

void Entity::CreateCollider(Shape shape)
{
    if (mpCollider != nullptr) delete mpCollider;
    // TODO : use OBB
    switch (shape)
    {
    case Shape::CUBE_AABB:
        mpCollider = new Collider(&mTransform, Collider::ColliderType::AABB);
        break;
    case Shape::CUBE_OBB:
        mpCollider = new Collider(&mTransform, Collider::ColliderType::OBB);
        break;
    case Shape::SPHERE:
        mpCollider = new Collider(&mTransform, Collider::ColliderType::SPHERE);
        break;
    default:
        mpCollider = new Collider(&mTransform, Collider::ColliderType::AABB);
        break;
    }
}

Collider* Entity::GetCollider()
{
    return mpCollider;
}

void Entity::CreateRigidBody()
{
    mpRigidBody = new RigidBody(&mTransform, 1);
}

RigidBody* Entity::GetRigidBody()
{
    return mpRigidBody;
}

void Entity::Render(UpdateCtx* pCtx)
{
    glm::mat4 World = mTransform.GetMatrix();
    auto pProgram = mMesh.ActivateRenderProgram();
    pProgram->SetUniformMat4x4(World, "u_World");
    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
    pProgram->SetUniformMat4x4(*pCtx->pVP * World, "u_WVP");
    pProgram->SetUniformTexture(0, mpTex, "u_Texture0");
    mMesh.Draw();
}

void Entity::RenderWireCollider(UpdateCtx* pCtx)
{
    glm::mat4 transform(1);
    if (mpCollider->Type == Collider::ColliderType::SPHERE)
    {
        glm::vec3 position = mTransform.GetPosition();
        float radius = mTransform.GetMaxScale();
        glm::vec3 scale = { radius, radius, radius };
        glm::vec3 YPR = mTransform.GetRotation();
        transform = Translate(position) * Scale(scale) * RotateYawPitchRoll(YPR);
    }
    else {
        transform = mTransform.GetMatrix();
    }

    auto pProgram = mWireColliderMesh.ActivateRenderProgram();
    pProgram->SetUniformMat4x4(transform, "u_World");
    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
    pProgram->SetUniformMat4x4(*pCtx->pVP * transform, "u_WVP");
    pProgram->SetUniformVec3((Is_Colliding ? glm::vec3{ 1, 0, 0 } : glm::vec3{ 1, 1, 1 }), "u_WireFrameColor");
    pProgram->SetUniformFloat32(mMaxRenderDistanceDebug, "u_MaxDistance");
    mWireColliderMesh.Draw();
}

void Entity::RenderPointedMesh(UpdateCtx* pCtx)
{
    //glm::mat4 WorldCollider = glm::scale(mTransform.GetMatrix(), { 1.01, 1.01, 1.01 });
    auto pProgram = mPointedMesh.ActivateRenderProgram();
    pProgram->SetUniformMat4x4(mTransform.GetMatrix(), "u_World");
    pProgram->SetUniformMat4x4(*pCtx->pView, "u_View");
    pProgram->SetUniformMat4x4(*pCtx->pProj, "u_Proj");
    pProgram->SetUniformMat4x4(*pCtx->pVP * mTransform.GetMatrix(), "u_WVP");
    pProgram->SetUniformVec3(glm::vec3{ 0, 0, 0 }, "u_WireFrameColor");
    pProgram->SetUniformFloat32(mMaxRenderDistanceDebug, "u_MaxDistance");
    mPointedMesh.Draw();
}

glm::vec3 Entity::GetBoundingBox()
{
    if (mpCollider->Type == Collider::ColliderType::SPHERE)
    {
        float max_scale = mTransform.GetMaxScale();
        return { max_scale, max_scale , max_scale };
    }

    glm::vec4 extent = glm::vec4(mTransform.GetScale(), 0.0f) * mTransform.GetMatrixRotation();
    return {
        abs(extent.x),
        abs(extent.y),
        abs(extent.z)
    };
}

