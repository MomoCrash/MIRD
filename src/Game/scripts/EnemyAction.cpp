#include "EnemyAction.h"

#include "BulletScript.h"
#include "RenderWindow.h"
#include "InputManager/InputManager.h"
#include "InputManager/Keycode.h"
#include "Resources.h"
#include "Transform.h"
#include "Core/Utils/Maths.h"
#include "ECS/ObjectFactory.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/RigidBody3D.h"
#include "ECS/Components/Colliders/SphereCollider.h"

void EnemyAction::OnCollisionEnter(Entity* other)
{
    TRANSFORM* transform = owner->GetTransform();

    if (other->IsTag(Entity::Tag::PROJECTILES))
    {
        owner->Destroy();
    }
}

void EnemyTrigger::OnFixedUpdate()
{
    if (player)
    {
        LookAtPlayer();
    }
}

void EnemyTrigger::OnTriggerStay(Entity* other)
{
    if (other->IsTag(Entity::Tag::PLAYER))
    {
        player = other;
        LookAtPlayer();
    }
}

void XM_CALLCONV EnemyTrigger::Shoot(FXMVECTOR position, FXMVECTOR forward)
{
    Entity* projectile = ObjectFactory::CreateEntity<Entity>();
    projectile->SetTag(Entity::Tag::ENEMY_PROJECTILES);
    XMVECTOR finalPosition = XMVectorAdd(position, { Maths::RandomFloat(0.0f, 0.5f), Maths::RandomFloat(0.0f, 0.5f), Maths::RandomFloat(0.0f, 0.5f) });
    finalPosition = XMVectorAdd(finalPosition, forward * 2);

    projectile->GetTransform()->SetPosition(finalPosition);
    projectile->GetTransform()->SetScale({ 1.0f, 1.0f, 1.0f });
    ObjectFactory::CreateComponent<MeshRenderer>(projectile, Resources::instance().MESH_DUCK, Resources::instance().MATERIAL_DUCK);
    ObjectFactory::CreateComponent<SphereCollider>(projectile, 1.0f);
    ObjectFactory::AttachScript<BulletScript>(projectile);
    RigidBody3D* rigidbody = ObjectFactory::CreateComponent<RigidBody3D>(projectile);
    rigidbody->ApplyImpulse(forward * 20.0f);
}

void EnemyTrigger::LookAtPlayer()
{
    
    TRANSFORM* transform = owner->GetTransform()->GetParent()->GetTransform();
    
    XMVECTOR ownerPos = XMLoadFloat3(&owner->GetTransform()->position);
    XMVECTOR otherPos = XMLoadFloat3(&player->GetTransform()->position);

    XMMATRIX lookAtM = XMMatrixLookAtLH(-ownerPos, otherPos, XMLoadFloat3(&owner->GetTransform()->up));

    XMVECTOR quaternionRotation = XMQuaternionRotationMatrix(lookAtM);

    transform->RotationFromQuaternion(quaternionRotation);
}
