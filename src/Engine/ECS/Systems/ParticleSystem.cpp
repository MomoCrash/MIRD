#include "pch.h"
#include "ParticleSystem.h"

#include "ECS/ECS.h"
#include "ECS/Components/ParticleEmitter.h"

ParticleSystem::ParticleSystem(RenderWindow* window) : mWindow(window) { }

void ParticleSystem::OnRender(ECS* globalEC, float dT)
{
    for (int i = 0; i < globalEC->mEntityCount; i++)
    {
        if (!globalEC->HasComponent<ParticleEmitter>(i)) continue;
        ParticleEmitter* particle = globalEC->GetComponent<ParticleEmitter>(i);
        if (particle->Start && !particle->InProgress)
        {
            TRANSFORM* transform = particle->GetEntity()->GetTransform();
            particle->Particle->StartAt(XMLoadFloat3(&transform->forward));
            particle->LifeTime = particle->MaxLifeTime;
            particle->InProgress = true;
            particle->Start = false;
        }
        if (particle->InProgress)
        {
            particle->LifeTime -= dT;
            if (particle->LifeTime < 0.0)
            {
                particle->InProgress = false;
                continue;
            }
            mWindow->ComputeParticle(particle->Particle, dT);
            EmitterDataSet dataSet;
            XMMATRIX position = XMMatrixTranslationFromVector(XMLoadFloat4(&particle->position));
            XMStoreFloat4x4(&dataSet.position, XMMatrixTranspose(position));
            particle->ParticleDataBuffer->CopyData(0, dataSet);
            particle->Particle->Render<EmitterDataSet>(particle->ParticleDataBuffer);   
        }
    }
}
