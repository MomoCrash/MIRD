#pragma once

class BoxCollider : public Collider3D
{
public:
    BoxCollider(Entity* entity, float xMin = -1.0f, float yMin = -1.0f, float zMin = -1.0f, float xMax = 1.0f, float yMax = 1.0f, float zMax = 1.0f, bool IsTrigger = false, bool IsStatic = false, const PhysicsMaterial* nMaterial = nullptr);

#pragma region Setters
    void SetMin(float x, float y, float z);
    void SetMax(float x, float y, float z);
    void SetSize(float width, float height, float depth);
    void SetHalfExtents();
#pragma endregion

#pragma region Getters
    XMVECTOR GetMin() const { return XMLoadFloat3(&mMin); }
    XMVECTOR GetMax() const { return XMLoadFloat3(&mMax); }
    XMVECTOR GetHalfExtents() const { return XMLoadFloat3(&mHalfExtents); }
#pragma endregion
    
private:
    XMFLOAT3 mMin;
    XMFLOAT3 mMax;
    XMFLOAT3 mHalfExtents;
};
