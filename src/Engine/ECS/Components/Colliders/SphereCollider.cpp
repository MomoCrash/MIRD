#include <pch.h>
#include "SphereCollider.h"
#include "ECS/Entity.h"

SphereCollider::SphereCollider(Entity* entity, float nRadius, bool IsTrigger, bool IsStatic, const PhysicsMaterial* nMaterial) : Collider3D(entity, IsTrigger, IsStatic, nMaterial)
{
    mRadius = nRadius;

    mColliderType = ColliderType::Sphere;
}
