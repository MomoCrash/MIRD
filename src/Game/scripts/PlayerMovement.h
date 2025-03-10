#pragma once
#include "scripts/Script.h"

class RigidBody3D;

class PlayerMovement : public IScript
{
    POINT lastMousePos ;
    Entity* camera;
    Entity* gun;
    RigidBody3D* rigidbody;

    XMFLOAT3 mDirection;

    float phi;
    float theta;
    
public:
    PlayerMovement() = default;
    
    void OnStart() override;
    void OnFixedUpdate() override;
    void OnPreRender() override;
    void OnUpdate() override;
    void OnDisable() override;
};
