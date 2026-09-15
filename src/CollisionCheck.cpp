#include "CollisionCheck.h"
#include "Entity.h"

bool CollisionCheck::Collide(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    return CollideFunc[(int)pEntity1->GetCollider()->Type][(int)pEntity2->GetCollider()->Type](pEntity1, pEntity2, impact);
}

bool CollisionCheck::CollideSphere_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    if (pEntity1->GetRigidBody() == nullptr && pEntity2->GetRigidBody() == nullptr)
    {
        return false; // NO COLLISION
    }

    Collider* pCollider1 = pEntity1->GetCollider();
    Collider* pCollider2 = pEntity2->GetCollider();
    Transform* pTransform1 = pCollider1->pTransform;
    Transform* pTransform2 = pCollider2->pTransform;

    if (pTransform1->GetPosition() == pTransform2->GetPosition()) return false; // IGNORE COLLISION IF PERFECT OVERLAP

    float radius1 = pTransform1->GetMaxScale();
    float radius2 = pTransform2->GetMaxScale();
    float sum_radii = radius1 + radius2;

    //glm::vec3 delta_center1_to_center2 = transform2.GetPosition() - transform1.GetPosition();
    glm::vec3 delta_center1_to_center2 = pTransform2->GetPosition() - pTransform1->GetPosition();
    float sq_distance = 
        delta_center1_to_center2.x * delta_center1_to_center2.x + 
        delta_center1_to_center2.y * delta_center1_to_center2.y + 
        delta_center1_to_center2.z * delta_center1_to_center2.z;

    if(sq_distance > sum_radii * sum_radii)
        return false; // NO COLLISION

    float distance = glm::sqrt(sq_distance);
    float inv_distance = 1.0f / distance;

    impact.normal = delta_center1_to_center2 * inv_distance;
    impact.penetration = sum_radii - distance;
    impact.collision_point_local_1 = impact.normal * radius1;
    impact.collision_point_local_2 = -impact.normal * radius2;
	impact.pEntity1 = pEntity1;
	impact.pEntity2 = pEntity2;    

    return true;
}

bool CollisionCheck::CollideSphere_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    return CollideAABB_Sphere(pEntity2, pEntity1, impact);
}

bool CollisionCheck::CollideSphere_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    // TODO : to implement
    return false; // NO COLLISION
}

bool CollisionCheck::CollideAABB_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    if (pEntity1->GetRigidBody() == nullptr && pEntity2->GetRigidBody() == nullptr)
    {
        return false; // NO COLLISION
    }

    Collider* pCollider1 = pEntity1->GetCollider();
    Collider* pCollider2 = pEntity2->GetCollider();
    Transform* pTransform1 = pCollider1->pTransform;
    Transform* pTransform2 = pCollider2->pTransform;

    if (pTransform1->GetPosition() == pTransform2->GetPosition()) return false; // IGNORE COLLISION IF PERFECT OVERLAP

    const glm::vec3& box_size = pTransform2->GetScale();
    float sphere_radius = pTransform1->GetMaxScale();

    glm::vec3 delta_center1_to_center2 = pTransform2->GetPosition() - pTransform1->GetPosition();
    glm::vec3 closest_point_on_box = glm::clamp(delta_center1_to_center2, -box_size, box_size);
    glm::vec3 local_box_point = delta_center1_to_center2 - closest_point_on_box;

    float sq_distance_to_closest_point =
        local_box_point.x * local_box_point.x +
        local_box_point.y * local_box_point.y +
        local_box_point.z * local_box_point.z;

    if (sq_distance_to_closest_point > sphere_radius * sphere_radius)
        return false; // NO COLLISION

    float distance_to_closest_point = sqrtf(sq_distance_to_closest_point);

    glm::vec3 collision_normal = { 0, 0, 0 };
    if (distance_to_closest_point == 0)
        collision_normal = glm::normalize(delta_center1_to_center2);
    else
        collision_normal = local_box_point / distance_to_closest_point;

    float penetration = sphere_radius - distance_to_closest_point;

    impact.collision_point_local_1 = { 0, 0, 0 };
    impact.collision_point_local_2 = -collision_normal * sphere_radius;
    impact.normal = collision_normal;
    impact.penetration = penetration;
    impact.pEntity1 = pEntity1;
    impact.pEntity2 = pEntity2;
    return true;
}

bool CollisionCheck::CollideAABB_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    if (pEntity1->GetRigidBody() == nullptr && pEntity2->GetRigidBody() == nullptr)
    {
        return false; // NO COLLISION
    }

    Collider* pCollider1 = pEntity1->GetCollider();
    Collider* pCollider2 = pEntity2->GetCollider();
    Transform* pTransform1 = pCollider1->pTransform;
    Transform* pTransform2 = pCollider2->pTransform;

    if (pTransform1->GetPosition() == pTransform2->GetPosition()) return false; // IGNORE COLLISION IF PERFECT OVERLAP

    glm::vec3 delta_center1_to_center2 = pTransform2->GetPosition() - pTransform1->GetPosition();
    glm::vec3 sum_scale = pTransform2->GetScale() + pTransform1->GetScale();

    
    if (
        abs(delta_center1_to_center2.x) > sum_scale.x ||
        abs(delta_center1_to_center2.y) > sum_scale.y ||
        abs(delta_center1_to_center2.z) > sum_scale.z
        ) return false; // NO COLLISION

    static glm::vec3 FACES[6] = {
        glm::vec3(-1,  0,  0), glm::vec3(1, 0, 0),
        glm::vec3( 0, -1,  0), glm::vec3(0, 1, 0),
        glm::vec3( 0,  0, -1), glm::vec3(0, 0, 1)
    };

    glm::vec3 max1 = pTransform1->GetPosition() + pTransform1->GetScale();
    glm::vec3 min1 = pTransform1->GetPosition() - pTransform1->GetScale();

    glm::vec3 max2 = pTransform2->GetPosition() + pTransform2->GetScale();
    glm::vec3 min2 = pTransform2->GetPosition() - pTransform2->GetScale();

    float distances[6] = {
        max2.x - min1.x,
        max1.x - min2.x,
        max2.y - min1.y,
        max1.y - min2.y,
        max2.z - min1.z,
        max1.z - min2.z,
    };

    float penetration = FLT_MAX;
    glm::vec3 bestAxis = { 0, 0, 0 };

    for (int i = 0; i < 6; ++i)
    {
        if (distances[i] < penetration) {
            penetration = distances[i];
            bestAxis = FACES[i];
        }
    }

    impact.collision_point_local_1 = { 0, 0, 0 };
    impact.collision_point_local_2 = { 0, 0, 0 };
    impact.normal = bestAxis;
    impact.penetration = penetration;
    impact.pEntity1 = pEntity1;
    impact.pEntity2 = pEntity2;
    return true;
}

bool CollisionCheck::CollideAABB_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    // TODO : to implement
    return false; // NO COLLISION
}

bool CollisionCheck::CollideOBB_Sphere(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    // TODO : to implement
    return false; // NO COLLISION
}

bool CollisionCheck::CollideOBB_AABB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    // TODO : to implement
    return false; // NO COLLISION
}
bool CollisionCheck::CollideOBB_OBB(Entity* pEntity1, Entity* pEntity2, Impact& impact)
{
    if (!pEntity1->GetRigidBody() && !pEntity2->GetRigidBody())
        return false;

    Collider* pCollider1 = pEntity1->GetCollider();
    Collider* pCollider2 = pEntity2->GetCollider();

    Transform* pTransform1 = pCollider1->pTransform;
    Transform* pTransform2 = pCollider2->pTransform;

    glm::vec3 pos1 = pTransform1->GetPosition();
    glm::vec3 pos2 = pTransform2->GetPosition();

    if (pos1 == pos2)
        return false;

    glm::vec3 scale1 = pTransform1->GetScale(); // demi-extents
    glm::vec3 scale2 = pTransform2->GetScale();

    // Passage dans l'espace local de l'OBB1
    glm::mat4 invRot1 = glm::inverse(pTransform1->GetMatrixRotation());
    glm::vec3 t = glm::vec3(invRot1 * glm::vec4(pos2 - pos1, 1.0f));

    // Matrice de rotation relative B -> A
    glm::mat4 R = pTransform2->GetMatrixRotation() * invRot1;
    glm::mat4 AbsR;

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            AbsR[i][j] = std::abs(R[i][j]) + FLT_EPSILON;

    // Variables SAT étendues
    float minPenetration = FLT_MAX;
    glm::vec3 bestAxis(0.0f);

    auto TestAxis = [&](float dist, float r1, float r2, const glm::vec3& axis)
        {
            float penetration = (r1 + r2) - std::abs(dist);
            if (penetration < 0.0f)
                return false;

            if (penetration < minPenetration)
            {
                minPenetration = penetration;
                bestAxis = axis * (dist < 0.0f ? -1.0f : 1.0f);
            }
            return true;
        };

    // Axes croisés Ai x Bj (9 axes)
    const glm::vec3 axesA[3] = {
        glm::normalize(glm::vec3(pTransform1->GetMatrixRotation()[0])),
        glm::normalize(glm::vec3(pTransform1->GetMatrixRotation()[1])),
        glm::normalize(glm::vec3(pTransform1->GetMatrixRotation()[2]))
    };

    const glm::vec3 axesB[3] = {
        glm::normalize(glm::vec3(pTransform2->GetMatrixRotation()[0])),
        glm::normalize(glm::vec3(pTransform2->GetMatrixRotation()[1])),
        glm::normalize(glm::vec3(pTransform2->GetMatrixRotation()[2]))
    };

    // Axes locaux OBB1 (A0, A1, A2)
    for (int i = 0; i < 3; ++i)
    {
        float r1 = scale1[i];
        float r2 = scale2.x * AbsR[i][0] + scale2.y * AbsR[i][1] + scale2.z * AbsR[i][2];

        if (!TestAxis(t[i], r1, r2, axesA[i]))
            return false;
    }

    // Axes locaux OBB2 (B0, B1, B2)
    for (int i = 0; i < 3; ++i)
    {
        float dist = t.x * R[0][i] + t.y * R[1][i] + t.z * R[2][i];

        float r1 = scale1.x * AbsR[0][i] + scale1.y * AbsR[1][i] + scale1.z * AbsR[2][i];
        float r2 = scale2[i];

        if (!TestAxis(dist, r1, r2, axesB[i]))
            return false;
    }

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            glm::vec3 axis = glm::cross(axesA[i], axesB[j]);
            float len2 = glm::dot(axis, axis);

            if (len2 < FLT_EPSILON)
                continue; // axes quasi parallèles

            axis = glm::normalize(axis);

            float dist =
                std::abs(t[(i + 2) % 3] * R[(i + 1) % 3][j] -
                    t[(i + 1) % 3] * R[(i + 2) % 3][j]);

            float r1 =
                scale1[(i + 1) % 3] * AbsR[(i + 2) % 3][j] +
                scale1[(i + 2) % 3] * AbsR[(i + 1) % 3][j];

            float r2 =
                scale2[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
                scale2[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

            if (!TestAxis(dist, r1, r2, axis))
                return false;
        }
    }

    // ===============================
    // COLLISION CONFIRMÉE
    // ===============================

    impact.normal = glm::normalize(bestAxis);
    impact.penetration = minPenetration;

    // Point de contact simple (suffisant pour impulsions + rotation)
    impact.collision_point_local_1 =
        pos1 + impact.normal * (scale1.x * 0.5f);

    impact.collision_point_local_2 =
        impact.collision_point_local_1 - impact.normal * impact.penetration;

    impact.pEntity1 = pEntity1;
    impact.pEntity2 = pEntity2;

    return true;
}