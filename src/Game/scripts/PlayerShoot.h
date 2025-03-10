#pragma once
#include "ECS/Components/ParticleEmitter.h"
#include "scripts/Script.h"

class PlayerShoot : public IScript
{
    float reloadDuration;
    float reloadProgress;
    
public:
    PlayerShoot() ;
    
    void OnStart() override;
    void OnUpdate() override;
    void XM_CALLCONV Shoot(FXMVECTOR position, XMVECTOR forward, float strength);
};
