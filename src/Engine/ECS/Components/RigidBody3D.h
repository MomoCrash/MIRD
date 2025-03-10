#pragma once
#include "libs/d3dUtils.h"
#include "Component.h"

class RigidBody3D : public Component
{
public:
    
    enum class RIGIDBODYTYPE : uint8_t
    {
        BOX,
        SPHERE,
        OBB,

        TYPE_COUNT
    };
    
    RigidBody3D(Entity* entity, bool UsingGravity = true, float nMass = 1.0f, float nDamping = 0.5f, RIGIDBODYTYPE nType = RIGIDBODYTYPE::TYPE_COUNT);
    
    static constexpr int BITMASK = 1 << 1;

#pragma region Values
    float mGravity;
    float mMass; //Weight in KG
    float mDamping;
    float mAngularDamping = 0.5f;     
    bool UseGravity;
    float mWidth;
    float mHeight;
    float mRadius;
    float mDepth;
    bool IsStatic;
    DirectX::XMFLOAT3X3 mInertiaTensor;
    RIGIDBODYTYPE mType;
    DirectX::XMFLOAT3 mVelocity;
    DirectX::XMFLOAT3 mTorque;
    DirectX::XMFLOAT4 mOrientation;
    DirectX::XMFLOAT3 mAngularVelocity;
    
protected:
    DirectX::XMFLOAT3 mAllForces = {0.0f,0.0f,0.0f};
#pragma endregion

#pragma region Functions
public:
    void SetType(RIGIDBODYTYPE nType);
    void SetSize(float w, float h, float d);
    void SetMass(float nMass);
    
    void XM_CALLCONV AddForce(DirectX::FXMVECTOR force);
    void XM_CALLCONV SubtractForce(DirectX::FXMVECTOR force);
    void AddForce(float x, float y, float z);
    DirectX::XMVECTOR GetForce() const;
    void ClearForces();
    int GetBitmask() override;
    void ClearVelocity();
    float GetInvMass() const { return 1/mMass; }
    DirectX::XMMATRIX RotationalInertia() const;
    DirectX::XMMATRIX GetInverseTensor() const;
    void XM_CALLCONV ApplyImpulse(DirectX::FXMVECTOR impulse);
    void XM_CALLCONV AddTorque(DirectX::FXMVECTOR torque);
#pragma endregion

};
