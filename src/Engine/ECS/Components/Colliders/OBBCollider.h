#pragma once

class OBBCollider : public Collider3D
{
public:
    OBBCollider(Entity* entity, bool IsTrigger = false, bool IsStatic = false, float xMin = -1.0f, float yMin = -1.0f, float zMin = -1.0f, float xMax = 1.0f, float yMax = 1.0f, float zMax = 1.0f);
    void SetSize(float width, float height, float depth);
    void SetMin(float x, float y, float z);
    void SetMax(float x, float y, float z);
    void SetHalfExtents();
    void UpdateAxes();

    DirectX::XMFLOAT3 mHalfExtents;
    DirectX::XMFLOAT3X3 mOrientation;

    XMFLOAT3 mAxes[3];

    XMFLOAT3 mMin;
    XMFLOAT3 mMax;

};
