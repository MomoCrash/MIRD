#pragma once
#include <windows.h>

#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/CollisionSystem.h"

#include "ECS/Managers/GameManager.h"
#include "ECS/Systems/ParticleSystem.h"
#include "InputManager/InputManager.h"

class ScriptManager;
class ObjectFactory;

class Engine
{
public:
    Engine();
    ~Engine();

    static void SetRenderWindow(HINSTANCE instance);
    static RenderWindow* GetRenderWindow();
    
    static GameManager* GetGameManager();
    static InputManager* GetInputManager();
    
    static float GetDeltaTime();

private:
    static Engine& instance()
    {
        static Engine game;
        return game;
    }

    static ECS* GetECS();
    static RenderSystem* GetRenderSystem();
    static CameraSystem* GetCameraSystem();
    static PhysicsSystem* GetPhysicsSystem();
    static CollisionSystem* GetCollisionSystem();
    static ParticleSystem* GetParticleSystem();
    static ScriptManager* GetScriptManager();

    GameManager* mGameManager;

    ECS* mECS;
    RenderSystem* mRenderSystem;
    PhysicsSystem* mPhysicsSystem;
    CollisionSystem* mCollisionSystem;
    CameraSystem* mCameraSystem;
    ParticleSystem* mParticleSystem;
    ScriptManager* mScriptManager;
    InputManager* mInputManager;

    RenderWindow* mRenderWindow;

    // Make system only accessible from engine
    friend Entity;
    friend ECS;
    
    friend ObjectFactory;
    
    friend GameManager;
    friend RenderSystem;
    friend PhysicsSystem;
    friend CollisionSystem;
    friend CameraSystem;
    friend ScriptManager;
};
