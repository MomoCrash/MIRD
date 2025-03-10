#include "pch.h"
#include "Collider3D.h"

#include "PhysicsMaterial.h"
#include "Colliders/SphereCollider.h"
#include "Colliders/BoxCollider.h"
#include "Colliders/UtilsCollider.h"
#include "ECS/Entity.h"
#include "ECS/ObjectFactory.h"

Collider3D::Collider3D(Entity* entity, bool IsTrigger, bool IsStatic, const PhysicsMaterial* nMaterial) : Component(entity)
{
    mColliderType = ColliderType::COLLIDER_COUNT;
    
    mIsTrigger = IsTrigger;
    mIsStatic = IsStatic;

    mCenter.x = entity->GetTransform()->position.x;
    mCenter.y = entity->GetTransform()->position.y;
    mCenter.z = entity->GetTransform()->position.z;

    mPhysicMaterial = ObjectFactory::CreateComponent<PhysicsMaterial>(entity);

    if(nMaterial != nullptr)
        mPhysicMaterial->SetNewMaterial(nMaterial);
        entity->GetComponent<PhysicsMaterial>()->SetNewMaterial(mPhysicMaterial);
}

CollisionManifold Collider3D::CheckCollisionSphereSphere(Collider3D* Collider)
{
    CollisionManifold manifold;
    SphereCollider* collider1 = static_cast<SphereCollider*>(this);
    SphereCollider* collider2 = static_cast<SphereCollider*>(Collider);

    XMVECTOR center1 = collider1->GetCenter();
    XMVECTOR center2 = collider2->GetCenter();
    XMVECTOR diff = center2 - center1;
    
    float dist = XMVectorGetX(XMVector3Length(diff));
    float radii = collider1->GetRadius() + collider2->GetRadius();
    
    if (dist <= (radii * radii)) {
        manifold.hasCollision = true;
        
        XMStoreFloat3(&manifold.normal, XMVector3Normalize(diff));

        XMVECTOR collisionNormal = XMLoadFloat3(&manifold.normal);
        
        manifold.penetrationDepth = radii - dist;
        
        float penetrationRatio = collider1->GetRadius() / radii;
        XMVECTOR contactPoint = center1 - collisionNormal * XMVectorReplicate(collider1->GetRadius() - manifold.penetrationDepth * penetrationRatio);
        
        XMFLOAT3 contactPointFloat3;
        XMStoreFloat3(&contactPointFloat3, contactPoint);
        manifold.AddContactPoint(contactPointFloat3);
    }
    
    return manifold;
}

CollisionManifold Collider3D::CheckCollisionSphereBox(Collider3D* Collider)
{
    CollisionManifold manifold;
    SphereCollider* sphere;
    BoxCollider* box;

    if (Collider->GetColliderType() == ColliderType::Sphere) {
        sphere = static_cast<SphereCollider*>(Collider);
        box = static_cast<BoxCollider*>(this);
    } else {
        sphere = static_cast<SphereCollider*>(this);
        box = static_cast<BoxCollider*>(Collider);
    }

    XMVECTOR spherePos = sphere->GetCenter();
    XMVECTOR boxCenter = box->GetCenter();

    // Trouver le point de la boîte le plus proche du centre de la sphère
    XMFLOAT3 closestP;
    
    float distSq = UtilsColliders::ClosestPtPointBoxCollider(sphere->mCenter, box, closestP);
    
    XMVECTOR closestPoint = XMLoadFloat3(&closestP);

    float dist = sqrtf(distSq);
    float sphereRadius = sphere->GetRadius();

    if (distSq <= (sphereRadius * sphereRadius)) {
        manifold.hasCollision = true;
        
        // Calculer la normale
        if (distSq > 1e-6f) {
            XMVECTOR diff = spherePos - closestPoint;
            XMStoreFloat3(&manifold.normal, XMVector3Normalize(-diff));
        } else {
            // Si le centre de la sphère est exactement sur le point le plus proche,
            // déterminer quelle face de la boîte est la plus proche
            XMVECTOR boxExtents = box->GetHalfExtents();
            XMVECTOR relativePosNormalized = (spherePos - boxCenter) / boxExtents;
            
            // Trouver la composante avec la plus grande valeur absolue
            XMFLOAT3 relPosFloat;
            XMStoreFloat3(&relPosFloat, relativePosNormalized);
            float absX = std::fabs(relPosFloat.x);
            float absY = std::fabs(relPosFloat.y);
            float absZ = std::fabs(relPosFloat.z);
            
            if (absX > absY && absX > absZ) {
               XMStoreFloat3(&manifold.normal, XMVector3Normalize(XMVectorSet(relPosFloat.x > 0 ? 1.0f : -1.0f, 0, 0, 0)));
            } else if (absY > absZ) {
                XMStoreFloat3(&manifold.normal, XMVector3Normalize(XMVectorSet(0, relPosFloat.y > 0 ? 1.0f : -1.0f, 0, 0)));
            } else {
                XMStoreFloat3(&manifold.normal, XMVector3Normalize(XMVectorSet(0, 0, relPosFloat.z > 0 ? 1.0f : -1.0f, 0)));
            }
        }
        
        // Calculer la profondeur de pénétration
        manifold.penetrationDepth = sphereRadius - dist;
        
        // Ajouter le point de contact
        XMFLOAT3 contactPointFloat3;
        XMStoreFloat3(&contactPointFloat3, closestPoint);
        manifold.AddContactPoint(contactPointFloat3);
    }
    
    return manifold;
}

CollisionManifold Collider3D::CheckCollisionBoxBox(Collider3D* Collider)
{
    CollisionManifold manifold;
    BoxCollider* box1 = static_cast<BoxCollider*>(this);
    BoxCollider* box2 = static_cast<BoxCollider*>(Collider);
    
    XMVECTOR box1Center = box1->GetCenter();
    XMVECTOR box2Center = box2->GetCenter();
    XMVECTOR box1HalfExtents = box1->GetHalfExtents();
    XMVECTOR box2HalfExtents = box2->GetHalfExtents();

    XMVECTOR distance = XMVectorAbs(XMVectorSubtract(box1Center, box2Center));
    XMVECTOR combinedExtents = XMVectorAdd(box1HalfExtents, box2HalfExtents);
    
    bool collisionX = XMVectorGetX(distance) <= XMVectorGetX(combinedExtents);
    bool collisionY = XMVectorGetY(distance) <= XMVectorGetY(combinedExtents);    
    bool collisionZ = XMVectorGetZ(distance) <= XMVectorGetZ(combinedExtents);

    if (collisionX && collisionY && collisionZ)
        {
        manifold.hasCollision = true;
        
       XMStoreFloat3(&manifold.normal, UtilsColliders::GetNormalBoxBox(box1, box2));

        manifold.penetrationDepth = UtilsColliders::GetPenetrationDepthBoxBox(box1,box2);
        
        XMVECTOR midpoint = (box1Center + box2Center) * 0.5f;
        
        XMFLOAT3 contactPointFloat3;
        XMStoreFloat3(&contactPointFloat3, midpoint);
        manifold.AddContactPoint(contactPointFloat3);
    }
    
    return manifold;
}

CollisionManifold Collider3D::Intersects(Collider3D* Collider)
{
    CollisionManifold manifold;
    ColliderType entity1 = this->GetColliderType();
    ColliderType entity2 = Collider->GetColliderType();

    switch (entity1)
    {
        case ColliderType::Sphere:
            if (entity2 == ColliderType::Sphere) return CheckCollisionSphereSphere(Collider);
            if (entity2 == ColliderType::AABB) return CheckCollisionSphereBox(Collider);
            break;

        case ColliderType::AABB:
            if (entity2 == ColliderType::Sphere) return CheckCollisionSphereBox(Collider);
            if (entity2 == ColliderType::AABB) return CheckCollisionBoxBox(Collider);
            break;
    }
    
    return manifold;
}

int Collider3D::GetBitmask()
{
    return BITMASK;
}



