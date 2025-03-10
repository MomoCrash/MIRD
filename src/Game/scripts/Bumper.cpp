#include "Bumper.h"

#include "ECS/Components/RigidBody3D.h"

void Bumper::OnTriggerEnter(Entity* other)
{
    if (other->IsTag(Entity::Tag::PLAYER))
    {
        other->GetComponent<RigidBody3D>()->ApplyImpulse({0.0f, 30.0f, 0.0f});
    }
}
