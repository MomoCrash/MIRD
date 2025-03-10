#include "pch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "UtilsCollider.h"

#include "ECS/Components/Collider3D.h"

BoxCollider::BoxCollider(Entity* entity ,float xMin, float yMin, float zMin, float xMax, float yMax, float zMax, bool IsTrigger, bool IsStatic, const PhysicsMaterial* nMaterial) : Collider3D(entity, IsTrigger, IsStatic, nMaterial)
{
    mColliderType = ColliderType::AABB;
    
    XMStoreFloat3(&mMin, {xMin,yMin,zMin});
    XMStoreFloat3(&mMax, {xMax,yMax,zMax});

    SetHalfExtents();
}

void BoxCollider::SetMin(float x, float y, float z)
{
    XMStoreFloat3(&mMin, {x,y,z});
    SetHalfExtents();
}

void BoxCollider::SetMax(float x, float y, float z)
{
    XMStoreFloat3(&mMax ,{x,y,z});
    SetHalfExtents();
}

void BoxCollider::SetHalfExtents()
{
    mHalfExtents.x = (mMax.x - mMin.x) * 0.5f;
    mHalfExtents.y = (mMax.y - mMin.y) * 0.5f;
    mHalfExtents.z = (mMax.z - mMin.z) * 0.5f;
}

void BoxCollider::SetSize(float width, float height, float depth)
{
    mHalfExtents.x = width * 0.5f;
    mHalfExtents.y = height * 0.5f;
    mHalfExtents.z = depth * 0.5f;

    XMStoreFloat3(&mMin, {mCenter.x-mHalfExtents.x,mCenter.y-mHalfExtents.y,mCenter.z-mHalfExtents.z});
    XMStoreFloat3(&mMax, {mCenter.x+mHalfExtents.x,mCenter.y+mHalfExtents.y,mCenter.z+mHalfExtents.z});
}