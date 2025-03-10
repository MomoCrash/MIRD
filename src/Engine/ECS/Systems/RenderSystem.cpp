#include "pch.h"
#include "RenderSystem.h"

#include "Transform.h"
#include "ECS/ECS.h"
#include "../Components/MeshRenderer.h"
#include "ECS/Components/Light.h"
#include "Utils/Debug.h"
RenderSystem::RenderSystem(RenderWindow* window): window(window) {}

void RenderSystem::Render(ECS* globalEC)
{
    bool hasLight = false;
    for (int i = 0; i < globalEC->mEntityCount; i++)
    {
        if (globalEC->HasComponent<MeshRenderer>(i))
        {
            MeshRenderer* renderer = globalEC->GetComponent<MeshRenderer>(i);
            TRANSFORM* transform = renderer->GetEntity()->GetTransform();
        
            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(transform->GetMatrix()));
            renderer->ObjConstantBuffer->CopyData(0, objConstants);
        
            window->Draw<ObjectConstants>(renderer->ObjMaterial, renderer->ObjMesh, renderer->ObjConstantBuffer);
        }
        if (globalEC->HasComponent<Light>(i))
        {
            Light* light = globalEC->GetComponent<Light>(i);
            TRANSFORM* transform = light->GetEntity()->GetTransform();
            window->ComputeLight(XMLoadFloat3(&transform->position), XMLoadFloat3(&light->Color), light->Strength);
        }
    }
}