#pragma once

class  SphereCollider : public Collider3D
{
public:
    SphereCollider(Entity* entity, float nRadius = 1.0f, bool IsTrigger = false, bool IsStatic = false, const PhysicsMaterial* nMaterial = nullptr);
    float GetRadius() const { return mRadius; }
    float mRadius;
    
};
