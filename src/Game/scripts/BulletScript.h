#pragma once
#include "scripts/Script.h"

struct ParticleEmitter;

class BulletScript : public IScript
{
    ParticleEmitter* emitter;
    float timeToLive;
public:
    BulletScript();
    void OnCollisionEnter(Entity* other) override;
    void OnStart() override;
    void OnUpdate() override;
    
};
