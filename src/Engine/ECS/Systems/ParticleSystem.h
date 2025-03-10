#pragma once

class ParticleSystem
{
public:
    ParticleSystem(RenderWindow* window);
    ~ParticleSystem() = default;
    
    void OnRender(ECS* globalEC, float dt);

private:
    RenderWindow* mWindow;
    
};
