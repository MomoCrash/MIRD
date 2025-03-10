#pragma once
#include "GameTimer.h"
#include "libs/Particules.h"

class Profiler;
class Scene;

class GameManager
{
public:
    ~GameManager();
    GameManager();

    Scene* mpActiveScene;

    int mWindowWidth;
    int mWindowHeight;

    float mAccumulator;

    GameTimer* GetTime();

private:
    void Run();

    void HandleInput();
    void FixedUpdate();
    void Update();
    void Draw();

    Profiler* mProfiler;
    GameTimer mTimer;
    bool IsStopped;

public:
    template<typename T>
    void LaunchScene();

    Scene* GetScene() const { return mpActiveScene; }
    
};

#include "Scene.h"

template <typename T>
void GameManager::LaunchScene()
{
	static_assert(std::is_base_of<Scene, T>::value, "T must be derived from Scene");

    _ASSERT(mpActiveScene == nullptr);

    T* newScene = new T();
    mpActiveScene = newScene;

    //INIT DE LA SCENE
    mpActiveScene->OnEnter();
    
    Run();
}
