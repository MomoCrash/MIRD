#include "pch.h"
#include "PhysicsSystem.h"

#include "ECS/ECS.h"
#include "ECS/Entity.h"
#include "ECS/Components/Collider3D.h"
#include "ECS/Components/PhysicsMaterial.h"
#include "ECS/Components/RigidBody3D.h"
#include "Utils/Profiler.h"

void PhysicsSystem::OnFixedUpdate(ECS* globalEC)
{
    for (int i = 0; i < globalEC->mEntityCount; i++)
    {
        if (!globalEC->HasComponent<RigidBody3D>(i)) continue;
        RigidBody3D* rb = globalEC->GetComponent<RigidBody3D>(i);
        if (rb->GetState() == false)
            continue;

        float deltaTime = mFixedTimestep;

        if (rb->UseGravity)
        {
            DirectX::XMVECTOR gravity = DirectX::XMVectorSet(0, -rb->mGravity * rb->mMass, 0, 0);
            rb->AddForce(gravity);
        }
        
        // Apply gravity and other forces
        DirectX::XMVECTOR force = rb->GetForce();
        DirectX::XMVECTOR acceleration = DirectX::XMVectorScale(force, rb->GetInvMass());

        // Update velocity with acceleration
        DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&rb->mVelocity);
        velocity = DirectX::XMVectorAdd(velocity, DirectX::XMVectorScale(acceleration, deltaTime));

        //Damping factor (Akin to Air Resistance IRL)
        velocity = DirectX::XMVectorScale(velocity, 1.0f - rb->mDamping * deltaTime);
        DirectX::XMStoreFloat3(&rb->mVelocity, velocity);

        // Update position based on velocity
        DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&rb->GetEntity()->GetTransform()->position);
        DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rb->mVelocity), deltaTime));
        rb->GetEntity()->GetTransform()->SetPosition(newPosition);
        rb->ClearForces();
    }
}


