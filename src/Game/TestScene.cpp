#include <iostream> 
#include "TestScene.h"
#include <Windows.h>


#include <DirectXMath.h>

#include "RenderWindow.h"
#include "ECS/Entity.h"
#include "Transform.h"
#include "ECS/ObjectFactory.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Light.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/ParticleEmitter.h"
#include "ECS/Components/RigidBody3D.h"
#include "ECS/Components/Colliders/BoxCollider.h"
#include "ECS/Components/Colliders/SphereCollider.h"

#include "libs/d3dUtils.h"
#include "libs/GeometryFactory.h"
#include "scripts/PlayerMovement.h"

void TestScene::OnEnter()
{

    Engine::GetRenderWindow()->OpenCommandList();

    Shader* basicShader = new Shader(L"../../res/shader/VS.hlsl", L"../../res/shader/PS.hlsl", "basicShader");
    basicShader->SetWireframe(false);

    basicShader->AddTextureView();
    basicShader->AddConstantBufferView();
    basicShader->AddConstantBufferView();
    
    basicShader->AddInputElement({"POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0});
    basicShader->AddInputElement({"NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 12});
    basicShader->AddInputElement({"TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 24 });
    basicShader->AddInputElement({"TEXCOORD", DXGI_FORMAT_R32G32B32_FLOAT, 36});

    ParticleShader* particle = new ParticleShader(Engine::GetRenderWindow()->GetDevice(), Engine::GetRenderWindow()->GetCommandList(),
    L"../../res/shader/PARTICLE.hlsl", L"../../res/shader/PARTICLE.hlsl", L"../../res/shader/CS.hlsl", "csTarget", 256);

    Texture2D* basicTexture = new Texture2D();
    basicTexture->filename = L"../../res/textures/dirt.dds";
    basicTexture->name = "default";

    Material* basicMaterial = new Material();
    basicMaterial->shader = basicShader;
    basicMaterial->texture = basicTexture;

    Material* secondMaterial = new Material();
    secondMaterial->shader = basicShader;
    secondMaterial->texture = basicTexture;
    
    Entity* ground = ObjectFactory::CreateEntity<Entity>();
    ground->GetTransform()->SetScale({50.0f, 1.0f, 50.0f});
    ground->GetTransform()->SetPosition({0.0f, -2.0f, 0.0f});
    ObjectFactory::CreateComponent<MeshRenderer>(ground, GeometryFactory::GetGrid(), basicMaterial);
    BoxCollider* groundCollider = ObjectFactory::CreateComponent<BoxCollider>(ground,-50.0f, 0.0f, -50.0f,50.0f, 1.0f, 50.0f,false,true);
    
    Entity* player = ObjectFactory::CreateEntity<Entity>();
    player->GetTransform()->SetPosition(0.0f, 10.0f, 0.0f);
    ObjectFactory::CreateComponent<MeshRenderer>(player, GeometryFactory::LoadGeometryFromFile(L"crystal.obj"), basicMaterial);
    ObjectFactory::CreateComponent<RigidBody3D>(player,false);
    ObjectFactory::CreateComponent<ParticleEmitter>(player, particle);
    ObjectFactory::CreateComponent<Light>(player, XMVectorSet(0.35f, 0.35f, 0.35f, 1.0f), 1.0f);
    SphereCollider* playerCollider = ObjectFactory::CreateComponent<SphereCollider>(player, 1.0f);

    ObjectFactory::AttachScript<PlayerMovement>(player);

    Entity* camera = ObjectFactory::CreateEntity<Entity>(player);
    camera->GetTransform()->SetLocalPosition({0.0f, 3.0f, -5.0f});
    camera->GetTransform()->Rotate(20.0f, 0.0f, 0.0f);
    ObjectFactory::CreateComponent<Camera>(camera);


    Engine::GetRenderWindow()->CloseCommandList();
    
}

void TestScene::OnUpdate()
{
}
