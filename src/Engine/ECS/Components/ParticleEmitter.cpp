#include "pch.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(Entity* entity, ParticleShader* shader, float lT)
    : Component(entity), Start(false), InProgress(false), MaxLifeTime(lT), Particle(shader), ParticleDataBuffer(nullptr),
      LifeTime(0.0f), position()
{
    ParticleDataBuffer = new UploadBuffer<EmitterDataSet>(Engine::GetRenderWindow()->GetDevice(), 1, true);
}

ParticleEmitter::~ParticleEmitter()
{
    ParticleDataBuffer->Resource()->Release();
}

int ParticleEmitter::GetBitmask()
{
    return BITMASK;
}
