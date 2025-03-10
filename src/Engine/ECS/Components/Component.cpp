#include "pch.h"
#include "Component.h"
#include "../Entity.h";

Component::Component(Entity* entity) : mEntity(entity)
{
    mActive = true;
}

Entity* Component::GetEntity() const
{
    return mEntity;
}

void Component::Activate()
{
    mActive = true;
}

void Component::Deactivate()
{
    mActive = false;
}

bool Component::GetState() const
{
    return mActive;
}