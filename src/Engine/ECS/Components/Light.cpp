#include "pch.h"
#include "Light.h"

Light::Light(Entity* entity, FXMVECTOR color, float strength) : Component(entity), Color(), Strength(strength)
{
    XMStoreFloat3(&Color, color);
}


int Light::GetBitmask()
{
    return BITMASK;
}
