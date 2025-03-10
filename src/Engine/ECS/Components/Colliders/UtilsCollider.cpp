#include "pch.h"
#include "UtilsCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

#pragma region Penetration

float UtilsColliders::ClosestPtPointBoxCollider(const XMFLOAT3& p, const BoxCollider* b, XMFLOAT3& q)
{
    XMVECTOR boxPosition = XMLoadFloat3(&b->GetEntity()->GetTransform()->position);

    XMVECTOR worldMin = b->GetMin() + boxPosition;
    XMVECTOR worldMax = b->GetMax() + boxPosition;

    XMVECTOR point = XMLoadFloat3(&p);

    XMVECTOR closestPoint = XMVectorClamp(point, worldMin, worldMax);

    XMStoreFloat3(&q, closestPoint);

    XMVECTOR diff = point - closestPoint;
    XMVECTOR sqDist = XMVector3LengthSq(diff);

    return XMVectorGetX(sqDist);
}

float UtilsColliders::GetPenetrationDepth(Collider3D* Collider, Collider3D* Collider2)
{
    if(Collider->GetColliderType() == Collider3D::ColliderType::Sphere)
    {
        SphereCollider* Sphere = static_cast<SphereCollider*>(Collider);

        if(Collider2->GetColliderType() == Collider3D::ColliderType::Sphere)
        {
            SphereCollider* Sphere2 = static_cast<SphereCollider*>(Collider2);
            
            return GetPeneterationDepthSphereSphere(Sphere, Sphere2);
        }

        if(Collider2->GetColliderType() == Collider3D::ColliderType::AABB)
        {
            BoxCollider* Box = static_cast<BoxCollider*>(Collider);

            return GetPenetrationDepthBoxSphere(Box, Sphere);
        }
    }

    if(Collider->GetColliderType() == Collider3D::ColliderType::AABB)
    {
        BoxCollider* Box = static_cast<BoxCollider*>(Collider);

        if(Collider2->GetColliderType() == Collider3D::ColliderType::AABB)
        {
            BoxCollider* Box2 = static_cast<BoxCollider*>(Collider2);

            return GetPenetrationDepthBoxBox(Box, Box2);
        }

        if(Collider2->GetColliderType() == Collider3D::ColliderType::Sphere)
        {
            SphereCollider* Sphere = static_cast<SphereCollider*>(Collider);

            return GetPenetrationDepthBoxSphere(Box, Sphere);
        }
    }
    return 0.0f;
}

float UtilsColliders::GetPeneterationDepthSphereSphere(const SphereCollider* SphereCollider1, const SphereCollider* SphereCollider2)
{
    DirectX::XMVECTOR pos1 = SphereCollider1->GetCenter();
    DirectX::XMVECTOR pos2 = SphereCollider2->GetCenter();
        
    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(pos2 - pos1));

    return (SphereCollider1->mRadius + SphereCollider2->mRadius) - distance;
}

#include <DirectXMath.h>

using namespace DirectX;

float UtilsColliders::GetPenetrationDepthBoxSphere(const BoxCollider* BoxCollider, const SphereCollider* SphereCollider)
{
    XMVECTOR boxPosition = XMLoadFloat3(&BoxCollider->GetEntity()->GetTransform()->position);
    XMVECTOR sphereCenter = SphereCollider->GetCenter();

    XMVECTOR boxMin = BoxCollider->GetMin() + boxPosition;
    XMVECTOR boxMax = BoxCollider->GetMax() + boxPosition;

    XMVECTOR closestPoint = XMVectorClamp(sphereCenter, boxMin, boxMax);

    XMVECTOR difference = closestPoint - sphereCenter;

    XMVECTOR distanceSquared = XMVector3LengthSq(difference); 
    float distance = XMVectorGetX(XMVectorSqrt(distanceSquared)); 

    return SphereCollider->mRadius - distance;
}

float UtilsColliders::GetPenetrationDepthBoxBox(const BoxCollider* BoxCollider1, const BoxCollider* BoxCollider2)
{
    XMVECTOR center1 = BoxCollider1->GetCenter();
    XMVECTOR center2 = BoxCollider2->GetCenter();
    
    XMVECTOR halfExtents1 = BoxCollider1->GetHalfExtents();
    XMVECTOR halfExtents2 = BoxCollider2->GetHalfExtents();
    
    XMVECTOR delta = XMVectorSubtract(center2, center1);

    XMVECTOR overlap = XMVectorSubtract(XMVectorAdd(halfExtents1, halfExtents2), XMVectorAbs(delta));

    XMFLOAT3 overlapValues;
    XMStoreFloat3(&overlapValues, overlap);

    if (overlapValues.x <= 0.0f || overlapValues.y <= 0.0f || overlapValues.z <= 0.0f)
    {
        return 0.0f; 
    }

    return min(overlapValues.x, min(overlapValues.y, overlapValues.z));
}

#pragma endregion

#pragma region OBB



#pragma endregion

#pragma region Normal

XMVECTOR UtilsColliders::GetNormal(Collider3D* Collider1, Collider3D* Collider2)
{
    Collider3D::ColliderType entity1 = Collider1->GetColliderType();
    Collider3D::ColliderType entity2 = Collider2->GetColliderType();
    
    if(entity1 == Collider3D::ColliderType::Sphere && entity2 == Collider3D::ColliderType::Sphere)
    {
        return GetNormalSphereSphere(Collider1, Collider2);
    }

    if(entity1 == Collider3D::ColliderType::AABB && entity2 == Collider3D::ColliderType::AABB)
    {
        return GetNormalBoxBox(Collider1, Collider2);
    }

    if(entity1 == Collider3D::ColliderType::Sphere && entity2 == Collider3D::ColliderType::AABB
        || entity2 == Collider3D::ColliderType::Sphere && entity1 == Collider3D::ColliderType::AABB)
    {
        return GetNormalBoxSphere(Collider1, Collider2);
    }

    return XMVectorZero();
}

XMVECTOR UtilsColliders::GetNormalSphereSphere(Collider3D* Collider1, Collider3D* Collider2)
{
    XMVECTOR pos1 = Collider1->GetCenter();
    XMVECTOR pos2 = Collider2->GetCenter();
    
    return DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pos2, pos1));
}

XMVECTOR UtilsColliders::GetNormalBoxSphere(Collider3D* Collider1, Collider3D* Collider2)
{
    SphereCollider* sphere = nullptr;
    BoxCollider* box = nullptr;
    
    if(Collider1->GetColliderType() == Collider3D::ColliderType::Sphere)
    {
        sphere = static_cast<SphereCollider*>(Collider1);
        box = static_cast<BoxCollider*>(Collider2);
    }
    else
    {
        sphere = static_cast<SphereCollider*>(Collider2);
        box = static_cast<BoxCollider*>(Collider1);
    }

    XMVECTOR sphereCenter = sphere->GetCenter();
    XMVECTOR boxMin = box->GetMin();
    XMVECTOR boxMax = box->GetMax();
    DirectX::XMVECTOR closestPoint =XMVectorClamp(sphereCenter, boxMin, boxMax);

    XMVECTOR normal = DirectX::XMVectorSubtract(sphereCenter,closestPoint);

    return XMVector3Normalize(normal);
}

XMVECTOR UtilsColliders::GetNormalBoxBox(Collider3D* Collider1, Collider3D* Collider2)
{
    BoxCollider* box1 = static_cast<BoxCollider*>(Collider1);
    BoxCollider* box2 = static_cast<BoxCollider*>(Collider2);

    XMVECTOR box1MinWorld = box1->GetMin() + XMLoadFloat3(&box1->GetEntity()->GetTransform()->position);
    XMVECTOR box1MaxWorld = box1->GetMax() + XMLoadFloat3(&box1->GetEntity()->GetTransform()->position);

    XMVECTOR box2MinWorld = box2->GetMin() + XMLoadFloat3(&box2->GetEntity()->GetTransform()->position);
    XMVECTOR box2MaxWorld =  box2->GetMax() + XMLoadFloat3(&box2->GetEntity()->GetTransform()->position);

    XMVECTOR box1CenterWorld = box1->GetCenter();
    XMVECTOR box2CenterWorld = box2->GetCenter();

    float overlapX = max(0.0f, min(XMVectorGetX(box1MaxWorld), XMVectorGetX(box2MaxWorld)) - max(XMVectorGetX(box1MinWorld), XMVectorGetX(box2MinWorld)));
    float overlapY = max(0.0f, min(XMVectorGetY(box1MaxWorld), XMVectorGetY(box2MaxWorld)) - max(XMVectorGetY(box1MinWorld), XMVectorGetY(box2MinWorld)));
    float overlapZ = max(0.0f, min(XMVectorGetZ(box1MaxWorld), XMVectorGetZ(box2MaxWorld)) - max(XMVectorGetZ(box1MinWorld), XMVectorGetZ(box2MinWorld)));

    if (overlapX <= 0.0f && overlapY <= 0.0f && overlapZ <= 0.0f)
    {
        return XMVectorZero();
    }

    float dirX = (XMVectorGetX(box1CenterWorld) < XMVectorGetX(box2CenterWorld)) ? -1.0f : 1.0f;
    float dirY = (XMVectorGetY(box1CenterWorld) < XMVectorGetY(box2CenterWorld)) ? -1.0f : 1.0f;
    float dirZ = (XMVectorGetZ(box1CenterWorld) < XMVectorGetZ(box2CenterWorld)) ? -1.0f : 1.0f;

    if (overlapX < overlapY && overlapX < overlapZ)
        {
        return XMVectorSet(dirX, 0.0f, 0.0f, 0.0f);
    } else if (overlapY < overlapZ)
        {
        return XMVectorSet(0.0f, dirY, 0.0f, 0.0f);
    } else
        {
        return XMVectorSet(0.0f, 0.0f, dirZ, 0.0f);
    }
}

#pragma endregion
