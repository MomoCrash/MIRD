#pragma once

#include "Component.h"
#include "libs/Particules.h"

struct EmitterDataSet
{
    XMFLOAT4X4 position;
};

struct ParticleEmitter : public Component
{
    ParticleEmitter(Entity* entity, ParticleShader* shader, float lT = 1.0f);
    ~ParticleEmitter();
    
    bool Start;
    bool InProgress;
    float MaxLifeTime;
    float LifeTime;

    XMFLOAT4 position;
    
    ParticleShader* Particle;
    static constexpr int BITMASK = 1 << 5;

    UploadBuffer<EmitterDataSet>* ParticleDataBuffer;
    
    int GetBitmask() override;
    
};
