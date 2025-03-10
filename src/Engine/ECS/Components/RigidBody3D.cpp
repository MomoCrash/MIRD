#include "pch.h"
#include "RigidBody3D.h"
#include "ECS/Systems/PhysicsSystem.h"

RigidBody3D::RigidBody3D(Entity* entity, bool UsingGravity, float nMass, float nDamping, RIGIDBODYTYPE nType) : Component(entity)
{
    mVelocity = {0.0f,0.0f, 0.0f};
    mMass = nMass;
    mGravity = 9.81f;
    UseGravity = UsingGravity;
    mDamping = nDamping;
    mWidth = 2.0f;
    mHeight = 2.0f;
    mDepth = 2.0f;
    mType = nType;
    mOrientation = {0.0f, 0.0f, 0.0f, 1.0f};
    mAngularVelocity = {0.0f, 0.0f, 0.0f};
    XMStoreFloat3x3(&mInertiaTensor, RotationalInertia());
}

void RigidBody3D::SetType(RIGIDBODYTYPE nType)
{
    mType = nType;
    XMStoreFloat3x3(&mInertiaTensor, RotationalInertia());
}

void RigidBody3D::SetSize(float w, float h, float d)
{
    mWidth = w;
    mHeight = h;
    mDepth = d;
    XMStoreFloat3x3(&mInertiaTensor, RotationalInertia());
}

void RigidBody3D::SetMass(float nMass)
{
    mMass = nMass;
    XMStoreFloat3x3(&mInertiaTensor, RotationalInertia());

}


void XM_CALLCONV RigidBody3D::AddForce(DirectX::XMVECTOR force)
{
    XMVECTOR allForces = DirectX::XMLoadFloat3(&mAllForces);
    allForces = DirectX::XMVectorAdd(allForces, force);
    XMStoreFloat3(&mAllForces, allForces);
}

void XM_CALLCONV RigidBody3D::SubtractForce(DirectX::FXMVECTOR force)
{
    XMVECTOR allForces = DirectX::XMLoadFloat3(&mAllForces);
    allForces = DirectX::XMVectorSubtract(allForces, force);
    XMStoreFloat3(&mAllForces, allForces);
}

void RigidBody3D::AddForce(float x, float y, float z)
{
    AddForce(DirectX::XMVectorSet(x, y, z, 0.0f));
}


DirectX::XMVECTOR RigidBody3D::GetForce() const
{
    return XMLoadFloat3(&mAllForces);
}

void RigidBody3D::ClearForces()
{
    mAllForces = {0.0f,0.0f,0.0f};
}

int RigidBody3D::GetBitmask()
{
    return BITMASK;
}

void RigidBody3D::ClearVelocity()
{
    mVelocity.x = 0.0f;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;
}

DirectX::XMMATRIX RigidBody3D::RotationalInertia() const
{

    DirectX::XMMATRIX matrix = XMMatrixIdentity();
    
    if(mMass <= 0.0f) return matrix;
    
    if(this->mType == RIGIDBODYTYPE::BOX || this->mType == RIGIDBODYTYPE::OBB)
    {
        float width2 = this->mWidth * this->mWidth;
        float height2 = this->mHeight * this->mHeight;
        float depth2 = this->mDepth * this->mDepth;

        float Ixx = (1.0f / 12.0f) * this->mMass * (height2 + depth2);
        float Iyy = (1.0f / 12.0f) * this->mMass * (width2 + depth2);
        float Izz = (1.0f / 12.0f) * this->mMass * (width2 + height2);

        matrix.r[0] = DirectX::XMVectorSet(Ixx, 0.0f, 0.0f, 0.0f);
        matrix.r[1] = DirectX::XMVectorSet(0.0f, Iyy, 0.0f, 0.0f);
        matrix.r[2] = DirectX::XMVectorSet(0.0f, 0.0f, Izz, 0.0f);

        if(this->mType == RIGIDBODYTYPE::OBB)
        {
            DirectX::XMMATRIX rotationMatrix = GetEntity()->GetTransform()->GetMatrix();
            DirectX::XMMATRIX rotationTranspose = XMMatrixTranspose(rotationMatrix);
            DirectX::XMMATRIX I_local = matrix;
            
            matrix = rotationMatrix * I_local * rotationTranspose;
        }
    }
    else if(this->mType == RIGIDBODYTYPE::SPHERE)
    {
        float I = 2.0f/5.0f * this->mMass * this->mRadius * this->mRadius;
        matrix.r[0] = DirectX::XMVectorSet(I, 0.0f, 0.0f, 0.0f);
        matrix.r[1] = DirectX::XMVectorSet(0.0f, I, 0.0f, 0.0f);
        matrix.r[2] = DirectX::XMVectorSet(0.0f, 0.0f, I, 0.0f);
    }

    return matrix;
}

DirectX::XMMATRIX RigidBody3D::GetInverseTensor() const
{
    XMVECTOR determinant;
    DirectX::XMMATRIX invTensor = XMMatrixInverse(&determinant, XMLoadFloat3x3(&mInertiaTensor));

    if (XMVectorGetX(XMVectorAbs(determinant))  < 1e-6f)
    {
        return DirectX::XMMatrixIdentity(); 
    }
    
    return invTensor;
}

void RigidBody3D::ApplyImpulse(DirectX::FXMVECTOR impulse)
{
    if (IsStatic || mMass == 0) return;

    XMVECTOR velocity = XMLoadFloat3(&mVelocity);
    
    XMStoreFloat3(&mVelocity, DirectX::XMVectorAdd(velocity, DirectX::XMVectorScale(impulse, GetInvMass()))) ;
}

void RigidBody3D::AddTorque(DirectX::XMVECTOR torque)
{
    XMVECTOR Torque = XMLoadFloat3(&mTorque);
    XMStoreFloat3(&mTorque, DirectX::XMVectorAdd(Torque, torque));
}
