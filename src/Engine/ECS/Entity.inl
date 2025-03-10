#pragma once

#include "ECS.h"
#include "Engine.h"

template <typename T>
T* Entity::GetComponent()
{
    return Engine::GetECS()->GetComponent<T>(this);
}

template <typename T>
bool Entity::HasComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");
    if (Bitmask & T::BITMASK) return true;
    return false;
}
