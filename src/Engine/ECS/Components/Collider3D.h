#pragma once
#include "Component.h"
#include "Colliders/CollisionManifold.h"
#include "Colliders/SpatialGrid.h"

struct PhysicsMaterial;

class Collider3D : public Component
{
public:
    static constexpr int BITMASK = 1 << 0;

    enum class ColliderType : uint8_t
    {
        AABB,
        Sphere,
        OBB,

        COLLIDER_COUNT
    };
    
Collider3D(Entity* entity, bool IsTrigger, bool IsStatic , const PhysicsMaterial* nMaterial = nullptr);

#pragma region ManifoldGeneration
    
    CollisionManifold Intersects(Collider3D* Collider);

    CollisionManifold CheckCollisionSphereSphere(Collider3D* Collider);
    CollisionManifold CheckCollisionBoxBox(Collider3D* Collider);
    
    CollisionManifold CheckCollisionSphereBox(Collider3D* Collider);
    
#pragma endregion

#pragma region Getters

    DirectX::XMVECTOR GetCenter() const { return XMLoadFloat3(&mCenter); }
    ColliderType GetColliderType() const { return mColliderType; }
    int GetBitmask() override;
    CellCoords GetLastGridPosition() const {return mLastGridPosition; }
    bool IsTrigger() const { return mIsTrigger; }
    bool IsStatic() const { return mIsStatic; }

#pragma endregion

#pragma region Setters
    
    void SetTrigger(bool answer) { mIsTrigger = answer; }
    void XM_CALLCONV SetCenter(DirectX::FXMVECTOR nCenter) { XMStoreFloat3(&mCenter, nCenter); ;}
    void SetLastGridPosition(const CellCoords& newPosition) { mLastGridPosition = newPosition; }    

#pragma endregion

#pragma region Values
    
protected:
    ColliderType mColliderType;
    CellCoords mLastGridPosition;
    bool mIsTrigger;
    bool mIsStatic;
    DirectX::XMFLOAT3 mCenter;
    PhysicsMaterial* mPhysicMaterial;
    
#pragma endregion
};
