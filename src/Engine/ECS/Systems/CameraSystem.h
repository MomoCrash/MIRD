#pragma once

class RenderWindow;
class ECS;

class CameraSystem
{
    
public:
    CameraSystem(RenderWindow* window);
    ~CameraSystem() = default;
    
    void Update(ECS* globalEC);

private:
    int mCurrentDisplayedCamera;
    RenderWindow* mWindow;
};
