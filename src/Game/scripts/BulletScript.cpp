#include "BulletScript.h"

#include "Transform.h"
#include "ECS/Components/ParticleEmitter.h"

BulletScript::BulletScript(): emitter(nullptr), timeToLive(10.0f)
{
}

void BulletScript::OnCollisionEnter(Entity* other)
{
    if (other->IsTag(Entity::Tag::GROUND))
    {
        emitter->Start = true;
        XMStoreFloat4(&emitter->position, XMLoadFloat3(&owner->GetTransform()->position) );
    }
}

void BulletScript::OnStart()
{
    emitter = owner->GetComponent<ParticleEmitter>();
    timeToLive = 10.0f;
}

void BulletScript::OnUpdate()
{
    timeToLive-=Engine::GetDeltaTime();
    if(timeToLive < 0.0f && !owner->IsDestroyed())
        owner->Destroy();
}
