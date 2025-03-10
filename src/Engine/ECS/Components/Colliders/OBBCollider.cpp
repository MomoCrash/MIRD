#include "pch.h"
#include "OBBCollider.h"

OBBCollider::OBBCollider(Entity* entity, bool IsTrigger, bool IsStatic, float xMin, float yMin, float zMin, float xMax, float yMax, float zMax) : Collider3D(entity, IsTrigger, IsStatic)
{
    mColliderType = ColliderType::OBB;
    
    mCenter = entity->GetTransform()->position;
    
    XMStoreFloat3(&mMin, {xMin,yMin,zMin});
    XMStoreFloat3(&mMax, {xMax,yMax,zMax});

    mAxes[0] = entity->GetTransform()->right;
    mAxes[1] = entity->GetTransform()->up;
    mAxes[2] = entity->GetTransform()->forward;

    XMStoreFloat3x3(&mOrientation, XMMatrixIdentity());
    SetHalfExtents();
    UpdateAxes();
    
}

void OBBCollider::SetSize(float width, float height, float depth)
{
    mHalfExtents.x = width * 0.5f;
    mHalfExtents.y = height * 0.5f;
    mHalfExtents.z = depth* 0.5f;

    XMStoreFloat3(&mMin, {mCenter.x-mHalfExtents.x,mCenter.y-mHalfExtents.y,mCenter.z-mHalfExtents.z});
    XMStoreFloat3(&mMax, {mCenter.x+mHalfExtents.x,mCenter.y+mHalfExtents.y,mCenter.z+mHalfExtents.z});
}

void OBBCollider::SetMin(float x, float y, float z)
{
    XMStoreFloat3(&mMin, {x,y,z});
    SetHalfExtents();
}

void OBBCollider::SetMax(float x, float y, float z)
{
    XMStoreFloat3(&mMax ,{x,y,z});
    SetHalfExtents();
}

void OBBCollider::SetHalfExtents()
{
    mHalfExtents.x = (mMax.x - mMin.x) * 0.5f;
    mHalfExtents.y = (mMax.y - mMin.y) * 0.5f;
    mHalfExtents.z = (mMax.z - mMin.z) * 0.5f;
}

void OBBCollider::UpdateAxes()
{
    // Récupérer la matrice de rotation de l'entité
    XMMATRIX matrix = GetEntity()->GetTransform()->GetMatrix();

    // Extraire les axes locaux de la matrice d'orientation
    XMVECTOR worldX = XMVectorSet(matrix.r[0].m128_f32[0], matrix.r[0].m128_f32[1], matrix.r[0].m128_f32[2], 0.0f);
    XMVECTOR worldY = XMVectorSet(matrix.r[1].m128_f32[0], matrix.r[1].m128_f32[1], matrix.r[1].m128_f32[2], 0.0f);
    XMVECTOR worldZ = XMVectorSet(matrix.r[2].m128_f32[0], matrix.r[2].m128_f32[1], matrix.r[2].m128_f32[2], 0.0f);

    // Normaliser les axes
    worldX = XMVector3Normalize(worldX);
    worldY = XMVector3Normalize(worldY);
    worldZ = XMVector3Normalize(worldZ);

    // Stocker les axes dans mAxes
    XMStoreFloat3(&mAxes[0], worldX);
    XMStoreFloat3(&mAxes[1], worldY);
    XMStoreFloat3(&mAxes[2], worldZ);

    XMStoreFloat3x3(&mOrientation, matrix);
    // Mettre à jour le centre de l'OBB
    mCenter = GetEntity()->GetTransform()->position;
}
