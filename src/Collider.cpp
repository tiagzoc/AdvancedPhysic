#include "Collider.h"

glm::vec3 Collider::GetHalfSize()
{
    if (Type == Collider::ColliderType::SPHERE)
    {
        float max_scale = pTransform->GetMaxScale();
        return { max_scale, max_scale, max_scale };
    }
    return pTransform->GetScale();
}
