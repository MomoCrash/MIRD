#pragma once
#include <vector>

#include "ECS/Entity.h"

const float Pi = 3.1415926535f;

struct TRANSFORM
{
private:
    bool mDirty;
    DirectX::XMFLOAT4X4 mMatrix;
    DirectX::XMFLOAT4X4 mRotation;
    DirectX::XMFLOAT4X4 mLocalRotation;
    
    std::vector<Entity*> mChildEntities;
    Entity* mpParentEntity;
    
public:
    TRANSFORM(Entity* npEntity);
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;

    DirectX::XMFLOAT4 rotation;
    DirectX::XMFLOAT4 localRotation;
    
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT3 localScale;

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 localPosition;

    Entity* GetParent() const;
    void AddChild(Entity* nEntity);
    std::vector<Entity*> GetChildList() const {return mChildEntities; }

    void Identity();
    void UpdateMatrix();

    XMMATRIX GetMatrix();
    void XM_CALLCONV FromMatrix(FXMMATRIX pMat);
    void XM_CALLCONV RotationFromQuaternion(FXMVECTOR pMat);
    
    void SetPosition(float x, float y, float z);
    
    void XM_CALLCONV SetLocalPosition(DirectX::FXMVECTOR pLocalVec);
    void XM_CALLCONV SetPosition(DirectX::FXMVECTOR pVec);

    void XM_CALLCONV SetLocalScale(DirectX::FXMVECTOR pLocalScale);
    void XM_CALLCONV SetScale(DirectX::FXMVECTOR pScale);

    void XM_CALLCONV SetLocalRotation(DirectX::FXMVECTOR pRotation);
    void XM_CALLCONV SetRotation(DirectX::FXMVECTOR pRotation);
    
    void Rotate(float pitch, float yaw, float roll);
    void RotatePitch(float angle);
    void RotateYaw(float angle);
    void RotateRoll(float angle);
};
