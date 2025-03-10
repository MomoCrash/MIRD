#pragma once
#include "BoxCollider.h"
#include "OBBCollider.h"
#include "SphereCollider.h"

namespace UtilsColliders
{
#pragma region Penetration
    float ClosestPtPointBoxCollider(const XMFLOAT3& p, const BoxCollider* b, XMFLOAT3& q);

    float GetPenetrationDepth(Collider3D* collider, Collider3D* Collider2);
    
    float GetPeneterationDepthSphereSphere(const SphereCollider* SphereCollider1, const SphereCollider* SphereCollider2);
    float GetPenetrationDepthBoxBox(const BoxCollider* BoxCollider1, const BoxCollider* BoxCollider2);
    float GetPenetrationDepthBoxSphere(const BoxCollider* BoxCollider, const SphereCollider* SphereCollider);

    
#pragma endregion

#pragma region Normal
    
    DirectX::XMVECTOR GetNormal(Collider3D* Collider1, Collider3D* Collider2);
    
    DirectX::XMVECTOR GetNormalSphereSphere(Collider3D* Collider1, Collider3D* Collider2);
    DirectX::XMVECTOR GetNormalBoxBox(Collider3D* Collider1, Collider3D* Collider2);

    DirectX::XMVECTOR GetNormalBoxSphere(Collider3D* Collider1, Collider3D* Collider2);

    
#pragma endregion
    


}

