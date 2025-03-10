#pragma once

class MeshCollider : public Collider3D
{
public:
    MeshCollider(Entity* entity, bool IsTrigger = false, bool IsStatic = false);
protected:
    std::vector<XMFLOAT3> mVertices;
    std::vector<uint32_t> mIndices;
};
