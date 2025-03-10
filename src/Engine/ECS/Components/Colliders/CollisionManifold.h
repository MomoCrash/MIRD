#pragma once

#include <DirectXMath.h>
#include <vector>

class Collider3D;

struct CollisionManifold
{
    Collider3D* collider1;
    Collider3D* collider2;
    bool hasCollision = false;
    DirectX::XMFLOAT3 normal;  
    float penetrationDepth = 0.0f;            
    std::vector<DirectX::XMFLOAT3> contactPoints; 

    void AddContactPoint(const DirectX::XMFLOAT3& point)
    {
        contactPoints.push_back(point);
    }

    bool operator<(const CollisionManifold& otherManifold) const
    {
        return this->penetrationDepth < otherManifold.penetrationDepth;
    }
};
