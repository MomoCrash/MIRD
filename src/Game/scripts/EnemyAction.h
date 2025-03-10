#pragma once
#include "scripts/Script.h"

class EnemyAction : public IScript
{

public:
    EnemyAction() = default;
    
    void OnCollisionEnter(Entity* other) override;
};

class EnemyTrigger : public IScript
{
    Entity* player;
    void OnFixedUpdate() override;
    void OnTriggerStay(Entity* other) override;
    void LookAtPlayer();
    void XM_CALLCONV Shoot(FXMVECTOR position, XMVECTOR forward);

public:
    EnemyTrigger() = default;
};