#include "pch.h"
#include "Engine.h"

#include "ECS/ECS.h"
#include "scripts/ScriptManager.h"

Engine::Engine() : mGameManager(new GameManager()), mECS(new ECS()), mRenderSystem(nullptr),
                   mPhysicsSystem(new PhysicsSystem()), mCollisionSystem(new CollisionSystem()), mCameraSystem(nullptr),
                   mParticleSystem(nullptr),
                   mScriptManager(new ScriptManager()), mInputManager(new InputManager()),
                   mRenderWindow(nullptr)
{
}

Engine::~Engine()
{
    delete mGameManager;

    delete mECS;
    delete mRenderSystem;
    delete mPhysicsSystem;
    delete mCollisionSystem;
    delete mCameraSystem;
    delete mParticleSystem;
    delete mScriptManager;
    delete mInputManager;

    delete mRenderWindow;
}


void Engine::SetRenderWindow(HINSTANCE windowInstance)
{
    RenderWindow* window = instance().mRenderWindow;
    if (window) return;
    
    window = new RenderWindow(windowInstance);
    window->Initialize();
    
    instance().mCameraSystem = new CameraSystem(window);
    instance().mRenderSystem = new RenderSystem(window);
    instance().mParticleSystem = new ParticleSystem(window);

    instance().mRenderWindow = window;
}

GameManager* Engine::GetGameManager()
{
    return instance().mGameManager;
}

CameraSystem* Engine::GetCameraSystem()
{
    return instance().mCameraSystem;
}

RenderWindow* Engine::GetRenderWindow()
{
    return instance().mRenderWindow;
}

RenderSystem* Engine::GetRenderSystem()
{
    return instance().mRenderSystem;
}

PhysicsSystem* Engine::GetPhysicsSystem()
{
    return instance().mPhysicsSystem;
}

CollisionSystem* Engine::GetCollisionSystem()
{
    return instance().mCollisionSystem;
}

ParticleSystem* Engine::GetParticleSystem()
{
    return instance().mParticleSystem;
}

ScriptManager* Engine::GetScriptManager()
{
    return instance().mScriptManager;
}

InputManager* Engine::GetInputManager()
{
    return instance().mInputManager;
}

float Engine::GetDeltaTime()
{
    return instance().mGameManager->GetTime()->GetDeltaTime();
}

ECS* Engine::GetECS()
{
    return instance().mECS;
}
