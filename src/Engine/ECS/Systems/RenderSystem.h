#pragma once

class RenderSystem
{
    
public:
    RenderSystem(RenderWindow* window);
    ~RenderSystem() = default;
    
    void Render(ECS* globalEC);

private:
    RenderWindow* window;
    
};
