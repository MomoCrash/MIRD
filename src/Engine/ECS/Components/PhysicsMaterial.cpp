#include "pch.h"
#include "PhysicsMaterial.h"

PhysicsMaterial::PhysicsMaterial(Entity* entity) : Component(entity)
{
}

void PhysicsMaterial::SetNewMaterial(const PhysicsMaterial* material)
{
    this->mElasticity = material->mElasticity;
    this->mDynamicFriction = material->mDynamicFriction;
    this->mStaticFriction = material->mStaticFriction;
}

int PhysicsMaterial::GetBitmask()
{
    return BITMASK;
}
