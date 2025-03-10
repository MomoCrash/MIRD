#include "pch.h"
#include "CameraSystem.h"

#include "ECS/ECS.h"
#include "ECS/Components/Camera.h"

CameraSystem::CameraSystem(RenderWindow* window): mWindow(window), mCurrentDisplayedCamera(0) { }

void CameraSystem::Update(ECS* globalEC)
{
    bool foundCamera = false;
    for (int i = 0; i < globalEC->mEntityCount; i++)
    {
        if (!globalEC->HasComponent<Camera>(i)) continue;
        Camera* camera = globalEC->GetComponent<Camera>(i);
        TRANSFORM* transform = camera->GetEntity()->GetTransform();
        mWindow->UpdateCameraConstants(XMLoadFloat3(&transform->position), transform->GetMatrix());
        foundCamera = true;
    }
    if (!foundCamera)
    {
        std::cout << "No camera found" << std::endl;
    }
}