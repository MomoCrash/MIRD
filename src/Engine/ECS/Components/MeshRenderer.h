#pragma once

#include "Component.h"
#include "Material.h"

struct Mesh;
struct ObjectConstants;

class MeshRenderer : public Component
{

public:
    static constexpr int BITMASK = 1 << 2;
    
    MeshRenderer(Entity* parent, Mesh* mesh, Material* material);
    MeshRenderer(Entity* parent);
    ~MeshRenderer() override;
    int GetBitmask() override;

    Mesh* ObjMesh;
    Material* ObjMaterial;
    UploadBuffer<ObjectConstants>* ObjConstantBuffer;
};
