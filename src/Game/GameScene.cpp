#include <iostream>
#include <random>
#include "GameScene.h"
#include <Windows.h>

#include <DirectXMath.h>
#include "Core/Utils/Profiler.h"

#include "RenderWindow.h"
#include "Resources.h"
#include "ECS/Entity.h"
#include "Transform.h"
#include "Core/Utils/Maths.h"
#include "ECS/ObjectFactory.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Light.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/ParticleEmitter.h"
#include "ECS/Components/PhysicsMaterial.h"
#include "ECS/Components/RigidBody3D.h"
#include "ECS/Components/Colliders/BoxCollider.h"
#include "ECS/Components/Colliders/SphereCollider.h"

#include "libs/GeometryFactory.h"
#include "scripts/PlayerMovement.h"
#include "scripts/PlayerShoot.h"
#include "scripts/TileScript.h"
#include "scripts/EnemyAction.h"

void GameScene::OnEnter()
{

    srand(static_cast<unsigned int>(time(nullptr)));

    Engine::GetRenderWindow()->OpenCommandList();

    Resources* resources = &Resources::instance();
    
    resources->CreateShaders();
    resources->CreateParticles();
    resources->CreateTextures();
    resources->CreateMaterials();
    resources->LoadCustomMesh();

    Engine::GetRenderWindow()->CloseCommandList();
    
    Entity* skybox = ObjectFactory::CreateEntity<Entity>();
    skybox->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);;
    skybox->GetTransform()->SetScale({200.0f, 200.0f, 200.0f});;
    ObjectFactory::CreateComponent<MeshRenderer>(skybox, Resources::instance().MESH_SKYBOX, resources->MATERIAL_SKYBOX);

    Entity* light = ObjectFactory::CreateEntity<Entity>();
    light->GetTransform()->SetPosition(50.0f, 80.0f, 50.0f);
    ObjectFactory::CreateComponent<Light>(light, XMVectorSet(0.35f, 0.35f, 0.0f, 1.0f), 50.0f);
    
    light = ObjectFactory::CreateEntity<Entity>();
    light->GetTransform()->SetPosition(-50.0f, 80.0f, -50.0f);
    ObjectFactory::CreateComponent<Light>(light, XMVectorSet(0.0f, 0.35f, 0.35f, 1.0f), 50.0f);

    light = ObjectFactory::CreateEntity<Entity>();
    light->GetTransform()->SetPosition(-50.0f, 80.0f, 0.0f);
    ObjectFactory::CreateComponent<Light>(light, XMVectorSet(0.35f, 0.0f, 0.35f, 1.0f), 50.0f);

    light = ObjectFactory::CreateEntity<Entity>();
    light->GetTransform()->SetPosition(0.0f, 80.0f, -50.0f);
    ObjectFactory::CreateComponent<Light>(light, XMVectorSet(0.35f, 0.35f, 0.35f, 1.0f), 50.0f);
    
    Entity* player = ObjectFactory::CreateEntity<Entity>();
    player->GetTransform()->SetPosition(0.0f, 150.0f, 0.0f);
    ObjectFactory::CreateComponent<RigidBody3D>(player);
    ObjectFactory::CreateComponent<BoxCollider>(player, -1.0f, -5.0f, -1.0f, 1.0f, 5.0f, 1.0f,false,false);
    PhysicsMaterial* material = player->GetComponent<PhysicsMaterial>();
    material->mElasticity = 0.0f;
    player->SetTag(Entity::Tag::PLAYER);
    
    ObjectFactory::AttachScript<PlayerMovement>(player);
    ObjectFactory::AttachScript<PlayerShoot>(player);
    
    Entity* camera = ObjectFactory::CreateEntity<Entity>(player);
    camera->GetTransform()->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
    ObjectFactory::CreateComponent<Camera>(camera);

    const int halfTerrain = 10;
    const int terrainRealSize = halfTerrain+halfTerrain;

    int lastHeightX = 5;
    int* lastHeightZ = new int [terrainRealSize];
    
    for (int x = -halfTerrain; x < halfTerrain; x++)
    {
        for (int z = -halfTerrain; z < halfTerrain; z++)
        {
            int finalHeight;
            if (x == 0)
            {
                lastHeightZ[halfTerrain + z] = 5;
                finalHeight = 5;
            } else
            {
                if (lastHeightZ[halfTerrain + z] == lastHeightX)
                    finalHeight = Maths::RandomInt(lastHeightX-1, lastHeightX+1) ;
                else
                    finalHeight = Maths::RandomInt(
                        lastHeightZ[halfTerrain + z] < lastHeightX ? lastHeightZ[halfTerrain + z] : lastHeightX,
                        lastHeightZ[halfTerrain + z] < lastHeightX ? lastHeightX : lastHeightZ[halfTerrain + z]);
            }
            if (finalHeight >= 8)
                finalHeight = 7;
            else if (finalHeight <= 4)
                finalHeight = 5;
            Entity* level = ObjectFactory::CreateEntity<Entity>();
            level->GetTransform()->SetScale({ 10.0f, 10.0f, 10.0f, 1.0f });
            level->GetTransform()->SetPosition( x * 10.0f, finalHeight * 10.0f, z * 10.0f);
            if (finalHeight <= 5)
            {
                ObjectFactory::CreateComponent<MeshRenderer>(level, GeometryFactory::GetCube(), resources->MATERIAL_DEFAULT);
                ObjectFactory::AttachScript<TileScript>(level);
            } else
                ObjectFactory::CreateComponent<MeshRenderer>(level, GeometryFactory::GetCube(), resources->MATERIAL_WALL);
            ObjectFactory::CreateComponent<BoxCollider>(level, -5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f,false,true);
            level->SetTag(Entity::Tag::GROUND);
            lastHeightX = finalHeight;
            lastHeightZ[halfTerrain + z] = finalHeight;
        }
    }
}

void GameScene::OnUpdate()
{

}