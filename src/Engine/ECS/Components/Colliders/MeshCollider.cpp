#include "pch.h"
#include "MeshCollider.h"

#include "ECS/Components/Collider3D.h"

MeshCollider::MeshCollider(Entity* entity, bool IsTrigger, bool IsStatic) : Collider3D(entity, IsTrigger, IsStatic)
{
    //mColliderType = ColliderType::Mesh;

    mVertices = { };
    mIndices = { };
    
}
