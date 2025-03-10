#include "pch.h"
#include "Transform.h"
#include "ECS/Entity.h"

TRANSFORM::TRANSFORM(Entity* npEntity) : mDirty(false)
{
    Identity();
    
    mpParentEntity = npEntity;
    mChildEntities = { };

    if (mpParentEntity != nullptr)
    {
        TRANSFORM* transform = mpParentEntity->GetTransform();
        SetPosition(XMLoadFloat3(&transform->position));
    }
    
}

void TRANSFORM::Identity()
{
    mMatrix._11 = 1.0f;
    mMatrix._12 = 0.0f;
    mMatrix._13 = 0.0f;
    mMatrix._14 = 0.0f;
    mMatrix._21 = 0.0f;
    mMatrix._22 = 1.0f;
    mMatrix._23 = 0.0f;
    mMatrix._24 = 0.0f;
    mMatrix._31 = 0.0f;
    mMatrix._32 = 0.0f;
    mMatrix._33 = 1.0f;
    mMatrix._34 = 0.0f;
    mMatrix._41 = 0.0f;
    mMatrix._42 = 0.0f;
    mMatrix._43 = 0.0f;
    mMatrix._44 = 1.0f;
    
    mRotation._11 = 1.0f;
    mRotation._12 = 0.0f;
    mRotation._13 = 0.0f;
    mRotation._14 = 0.0f;
    mRotation._21 = 0.0f;
    mRotation._22 = 1.0f;
    mRotation._23 = 0.0f;
    mRotation._24 = 0.0f;
    mRotation._31 = 0.0f;
    mRotation._32 = 0.0f;
    mRotation._33 = 1.0f;
    mRotation._34 = 0.0f;
    mRotation._41 = 0.0f;
    mRotation._42 = 0.0f;
    mRotation._43 = 0.0f;
    mRotation._44 = 1.0f;
    
    mLocalRotation._11 = 1.0f;
    mLocalRotation._12 = 0.0f;
    mLocalRotation._13 = 0.0f;
    mLocalRotation._14 = 0.0f;
    mLocalRotation._21 = 0.0f;
    mLocalRotation._22 = 1.0f;
    mLocalRotation._23 = 0.0f;
    mLocalRotation._24 = 0.0f;
    mLocalRotation._31 = 0.0f;
    mLocalRotation._32 = 0.0f;
    mLocalRotation._33 = 1.0f;
    mLocalRotation._34 = 0.0f;
    mLocalRotation._41 = 0.0f;
    mLocalRotation._42 = 0.0f;
    mLocalRotation._43 = 0.0f;
    mLocalRotation._44 = 1.0f;

    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;

    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;

    localPosition.x = 0.0f;
    localPosition.y = 0.0f;
    localPosition.z = 0.0f;

    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;

    localRotation.x = 0.0f;
    localRotation.y = 0.0f;
    localRotation.z = 0.0f;
    localRotation.w = 1.0f;

    forward.x = 0.0f;
    forward.y = 0.0f;
    forward.z = 1.0f;
    
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    right.x = 1.0f;
    right.y = 0.0f;
    right.z = 0.0f;
}

void XM_CALLCONV TRANSFORM::FromMatrix(DirectX::FXMMATRIX pMat)
{
    XMStoreFloat4x4(&mMatrix, pMat);
}

void XM_CALLCONV TRANSFORM::RotationFromQuaternion(FXMVECTOR quater)
{
    XMStoreFloat4x4(&mRotation, XMMatrixRotationQuaternion(quater));
    mDirty = true;
}

void TRANSFORM::UpdateMatrix()
{

    if (mDirty == false) return;
    mDirty = false;
    
    XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
    XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position));
    XMMATRIX rotation = DirectX::XMLoadFloat4x4(&mRotation);

    XMMATRIX matrix = XMMatrixIdentity();
    matrix = XMMatrixMultiply(matrix, scalingMatrix);
    matrix = XMMatrixMultiply(matrix, rotation);
    matrix = XMMatrixMultiply(matrix, translationMatrix);
    
    XMStoreFloat4x4(&mMatrix, matrix);
}

DirectX::XMMATRIX TRANSFORM::GetMatrix()
{
    return XMLoadFloat4x4(&mMatrix);
}

void XM_CALLCONV TRANSFORM::SetLocalPosition(DirectX::FXMVECTOR pVec)
{
    XMStoreFloat3(&localPosition, pVec);
    XMStoreFloat3(&position, pVec + XMLoadFloat3(&localPosition));
    for (Entity* child : mChildEntities)
    {
        child->GetTransform()->SetPosition(pVec);
    }
    mDirty = true;
}

void TRANSFORM::SetPosition(float x, float y, float z)
{
    XMVECTOR vector = XMVectorSet(x, y, z, 1.0f);
    XMStoreFloat3(&position, vector );
    for (Entity* child : mChildEntities)
    {
        child->GetTransform()->SetPosition(vector);
    }
    mDirty = true;
}

void XM_CALLCONV TRANSFORM::SetPosition(DirectX::FXMVECTOR pVec)
{
    XMStoreFloat3(&position, pVec);
    for (Entity* child : mChildEntities)
    {
        child->GetTransform()->SetPosition(pVec);
    }
    mDirty = true;
}

void XM_CALLCONV TRANSFORM::SetLocalScale(DirectX::FXMVECTOR pLocalScale)
{
    XMStoreFloat3(&localScale, pLocalScale);
    mDirty = true;
}

void XM_CALLCONV TRANSFORM::SetScale(DirectX::FXMVECTOR pScale)
{
    XMStoreFloat3(&scale, pScale);
    mDirty = true;
}

void XM_CALLCONV TRANSFORM::SetLocalRotation(DirectX::FXMVECTOR pRotation)
{
    XMMATRIX matrix = DirectX::XMMatrixRotationRollPitchYawFromVector(pRotation);
    XMStoreFloat4x4(&mLocalRotation, matrix);
    XMMATRIX globalRotation = matrix + XMLoadFloat4x4(&mLocalRotation);
    XMStoreFloat4x4(&mRotation, globalRotation);

    right.x = mRotation._11;
    right.y = mRotation._12;
    right.z = mRotation._13;

    up.x = mRotation._21;
    up.y = mRotation._22;
    up.z = mRotation._23;
    
    forward.x = mRotation._31;
    forward.y = mRotation._32;
    forward.z = mRotation._33;
    
    for (Entity* child : mChildEntities)
    {
        child->GetTransform()->SetRotation(pRotation);
    }
    mDirty = true;
}

void TRANSFORM::SetRotation(DirectX::FXMVECTOR pRotation)
{
    DirectX::XMStoreFloat4x4(&mRotation, DirectX::XMMatrixRotationRollPitchYawFromVector(pRotation) + XMLoadFloat4x4(&mLocalRotation));

    right.x = mRotation._11;
    right.y = mRotation._12;
    right.z = mRotation._13;

    up.x = mRotation._21;
    up.y = mRotation._22;
    up.z = mRotation._23;
    
    forward.x = mRotation._31;
    forward.y = mRotation._32;
    forward.z = mRotation._33;

    for (Entity* child : mChildEntities)
    {
        child->GetTransform()->SetRotation(pRotation);
    }
    
    mDirty = true;
}

void TRANSFORM::Rotate(float pitch, float yaw, float roll)
{
    mDirty = true;
    
    pitch *= DirectX::XM_PI/180;
    yaw *= DirectX::XM_PI/180;
    roll *= DirectX::XM_PI/180;
    DirectX::XMVECTOR currentRotation = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR qTemp;
    
    // Pitch
    qTemp = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&right), pitch);
    currentRotation = DirectX::XMQuaternionMultiply(currentRotation, qTemp);

    // Yaw
    qTemp = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), yaw);
    currentRotation = DirectX::XMQuaternionMultiply(currentRotation, qTemp);
    
    // Roll
    qTemp = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&forward), roll);
    currentRotation = DirectX::XMQuaternionMultiply(currentRotation, qTemp);

    // Multiply quaternion with rotation 
    DirectX::XMVECTOR objectRotation = DirectX::XMLoadFloat4(&rotation);
    objectRotation = DirectX::XMQuaternionMultiply(objectRotation, currentRotation);
    DirectX::XMStoreFloat4(&rotation, objectRotation);

    // Rotate matrix
    DirectX::XMStoreFloat4x4(&mRotation, DirectX::XMMatrixRotationQuaternion(objectRotation));

    right.x = mRotation._11;
    right.y = mRotation._12;
    right.z = mRotation._13;

    up.x = mRotation._21;
    up.y = mRotation._22;
    up.z = mRotation._23;
    
    forward.x = mRotation._31;
    forward.y = mRotation._32;
    forward.z = mRotation._33;
}

void TRANSFORM::RotateYaw(float angle)
{
    Rotate(0.0f, angle, 0.0f);
}

void TRANSFORM::RotatePitch(float angle)
{
    Rotate(angle, 0.0f, 0.0f);
}

void TRANSFORM::RotateRoll(float angle)
{
    Rotate(0.0f, 0.0f, angle);
}

Entity* TRANSFORM::GetParent() const
{
    return mpParentEntity;
}

void TRANSFORM::AddChild(Entity* nEntity)
{
    mChildEntities.push_back(nEntity);
}