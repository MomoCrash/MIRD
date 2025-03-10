#pragma once

#include "Component.h"

struct PhysicsMaterial : Component
{
    static constexpr int BITMASK = 1 << 4;
    
    PhysicsMaterial(Entity* entity);
    void SetNewMaterial(const PhysicsMaterial* material);
    int GetBitmask() override;
    
    float mElasticity = 0.9f;
    
    //Not Used
    float mStaticFriction = 0.6f;
    float mDynamicFriction = 0.6f;
};