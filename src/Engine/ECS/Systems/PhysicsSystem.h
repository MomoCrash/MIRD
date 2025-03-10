#pragma once

class PhysicsSystem
{
    float mFixedTimestep = 1.0f / 60.0f;
    
public:
    PhysicsSystem() = default;
    void OnFixedUpdate(ECS* globalEC);

};
