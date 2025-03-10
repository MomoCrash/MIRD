#include "pch.h"
#include "MeshRenderer.h"

#include "Utils/Debug.h"

MeshRenderer::MeshRenderer(Entity* parent, Mesh* mesh, Material* material) : Component(parent),
                                                                 ObjMesh(mesh), ObjMaterial(material),
                                                                 ObjConstantBuffer(nullptr)
{
    RenderWindow* render = Engine::GetRenderWindow();
    ObjConstantBuffer =  new UploadBuffer<ObjectConstants>(render->GetDevice(), 1, true);
    ObjConstantBuffer->Resource()->SetName(L"MESH_RENDERER");

    if (material == nullptr)
        ObjMaterial = Engine::GetRenderWindow()->GetDefaultMaterial();
    else if (material->CreateMaterial(render) == false)
    {
        Debug::Error("Failed to create material setting default");
    }
}

MeshRenderer::MeshRenderer(Entity* parent) : Component(parent), ObjMesh(nullptr), ObjMaterial(nullptr), ObjConstantBuffer(nullptr)
{
    ObjConstantBuffer =  new UploadBuffer<ObjectConstants>(Engine::GetRenderWindow()->GetDevice(), 1, true);
    ObjConstantBuffer->Resource()->SetName(L"MESH_RENDERER");
    ObjMaterial = Engine::GetRenderWindow()->GetDefaultMaterial();
}

MeshRenderer::~MeshRenderer()
{
    ObjConstantBuffer->Resource()->Release();
}

int MeshRenderer::GetBitmask()
{
    return BITMASK;
}
