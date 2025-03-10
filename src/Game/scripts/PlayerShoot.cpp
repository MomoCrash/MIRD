#include "PlayerShoot.h"

#include "BulletScript.h"
#include "Resources.h"
#include "Transform.h"
#include "Core/Utils/Maths.h"
#include "ECS/ObjectFactory.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/RigidBody3D.h"
#include "ECS/Components/Colliders/SphereCollider.h"
#include "InputManager/Keycode.h"

PlayerShoot::PlayerShoot(): reloadDuration(0.3f), reloadProgress(0)
{
}

void PlayerShoot::OnStart()
{
}

void PlayerShoot::OnUpdate()
{
    if (Engine::GetInputManager()->IsMouseButtonJustPressed(MOUSE_LEFT) && reloadProgress <= 0)
    {
        XMVECTOR position = XMLoadFloat3(&owner->GetTransform()->position);
        XMVECTOR forward = XMLoadFloat3(&owner->GetTransform()->forward);
        Shoot(position, forward, 50.0f);
        reloadProgress = reloadDuration;
    }
    
    reloadProgress -= Engine::GetDeltaTime();
}

void XM_CALLCONV PlayerShoot::Shoot(FXMVECTOR position, FXMVECTOR forward, float strength)
{
    Entity* projectile = ObjectFactory::CreateEntity<Entity>();
    projectile->SetTag(Entity::Tag::PROJECTILES);
    XMVECTOR finalPosition = XMVectorAdd(position, {Maths::RandomFloat(0.0f, 0.5f), Maths::RandomFloat(0.0f, 0.5f), Maths::RandomFloat(0.0f, 0.5f)});
    finalPosition = XMVectorAdd(position, forward*2);
    projectile->GetTransform()->SetPosition(finalPosition);
    projectile->GetTransform()->SetScale({5.0f, 5.0f, 5.0f});
    ObjectFactory::CreateComponent<MeshRenderer>(projectile, Resources::instance().MESH_DUCK, Resources::instance().MATERIAL_DUCK);
    ObjectFactory::CreateComponent<SphereCollider>(projectile, 1.0f);
    ObjectFactory::CreateComponent<ParticleEmitter>(projectile, Resources::instance().PARTICLE_WALL_EXPLOSION, 50.0f);
    ObjectFactory::AttachScript<BulletScript>(projectile);
    RigidBody3D* rigidbody = ObjectFactory::CreateComponent<RigidBody3D>(projectile);
    rigidbody->ApplyImpulse(forward*strength);
}
