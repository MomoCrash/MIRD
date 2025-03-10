#include "Resources.h"

#include "Engine.h"
#include "Material.h"
#include "RenderWindow.h"
#include "Shader.h"
#include "libs/Particules.h"

Resources::~Resources()
{
    ////////////////////////////////
    /////// MESH

    delete MESH_WATER_GUN;
    delete MESH_DUCK;
    delete MESH_SKYBOX;
    delete MESH_EYE;

    ////////////////////////////////
    /////// SHADER

    delete SHADER_BASIC;
    delete SHADER_SKYBOX;

    ////////////////////////////////
    /////// PARTICLES
    
    delete PARTICLE_DEFAULT_PARTICLE ;
    delete PARTICLE_WALL_EXPLOSION ;

    ////////////////////////////////
    /////// TEXTURES

    delete TEXTURE_GRASS;
    delete TEXTURE_WALL;
    delete TEXTURE_DUCK;
    delete TEXTURE_SKYBOX;
    delete TEXTURE_EYE;
    delete TEXTURE_BOUCEPAD;

    ////////////////////////////////
    /////// MATERIALS
    
    delete MATERIAL_DEFAULT ;
    delete MATERIAL_WALL ;
    delete MATERIAL_WATERGUN;
    delete MATERIAL_SKYBOX;
    delete MATERIAL_DUCK;
    delete MATERIAL_EYE;
    delete MATERIAL_DEFAULT;
}

void Resources::LoadCustomMesh()
{
    MESH_WATER_GUN = GeometryFactory::LoadGeometryFromFile(L"waterGun.obj");
    MESH_DUCK = GeometryFactory::LoadGeometryFromFile(L"duck.obj");
    MESH_SKYBOX = GeometryFactory::LoadGeometryFromFile(L"skybox.obj");
    MESH_EYE = GeometryFactory::LoadGeometryFromFile(L"eyeball.obj");
}

void Resources::CreateShaders()
{

    SHADER_BASIC = new Shader(L"VS.hlsl", L"PS.hlsl", "basicShader");
    SHADER_BASIC->SetInteriorView(false);
    SHADER_BASIC->SetWireframe(false);

    SHADER_BASIC->AddTextureView();
    SHADER_BASIC->AddConstantBufferView();
    SHADER_BASIC->AddConstantBufferView();

    SHADER_BASIC->AddInputElement({ "POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0 });
    SHADER_BASIC->AddInputElement({ "NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 12 });
    SHADER_BASIC->AddInputElement({ "TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 24 });
    SHADER_BASIC->AddInputElement({ "TEXCOORD", DXGI_FORMAT_R32G32B32_FLOAT, 36 });

    SHADER_SKYBOX = new Shader(L"skybox.hlsl", L"skybox.hlsl", "skyboxShader.cso");
    SHADER_SKYBOX->SetInteriorView(true);
    SHADER_SKYBOX->SetWireframe(false);

    SHADER_SKYBOX->AddTextureView();
    SHADER_SKYBOX->AddConstantBufferView();
    SHADER_SKYBOX->AddConstantBufferView();

    SHADER_SKYBOX->AddInputElement({ "POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0 });
    SHADER_SKYBOX->AddInputElement({ "NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 12 });
    SHADER_SKYBOX->AddInputElement({ "TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 24 });
    SHADER_SKYBOX->AddInputElement({ "TEXCOORD", DXGI_FORMAT_R32G32B32_FLOAT, 36 });
    
}

void Resources::CreateParticles()
{
    PARTICLE_DEFAULT_PARTICLE = new ParticleShader(Engine::GetRenderWindow()->GetDevice(), Engine::GetRenderWindow()->GetCommandList(),
        L"PARTICLE.hlsl", L"PARTICLE.hlsl", L"CS.hlsl", "particle.cso", 8192);

    PARTICLE_WALL_EXPLOSION = new ParticleShader(Engine::GetRenderWindow()->GetDevice(), Engine::GetRenderWindow()->GetCommandList(),
    L"PARTICLE.hlsl", L"PARTICLE.hlsl", L"CS.hlsl", "particle1.cso", 33554432);
}

void Resources::CreateTextures()
{
    
    TEXTURE_GRASS = new Texture2D();
    TEXTURE_GRASS->filename = L"dirt.dds";
    TEXTURE_GRASS->name = "default";

    TEXTURE_WALL = new Texture2D();
    TEXTURE_WALL->filename = L"wall.dds";
    TEXTURE_WALL->name = "wall";

    TEXTURE_WATERGUN = new Texture2D();
    TEXTURE_WATERGUN->filename = L"watergun.dds";
    TEXTURE_WATERGUN->name = "watergun";

    TEXTURE_DUCK = new Texture2D();
    TEXTURE_DUCK->filename = L"duck.dds";
    TEXTURE_DUCK->name = "duck";

    TEXTURE_SKYBOX = new Texture2D();
    TEXTURE_SKYBOX->filename = L"skybox.dds";
    TEXTURE_SKYBOX->name = "skybox";

    TEXTURE_EYE = new Texture2D();
    TEXTURE_EYE->filename = L"eye.dds";
    TEXTURE_EYE->name = "eye";

    TEXTURE_BOUCEPAD = new Texture2D();
    TEXTURE_BOUCEPAD->filename = L"boucepad.dds";
    TEXTURE_BOUCEPAD->name = "boucepad";
    
}

void Resources::CreateMaterials()
{

    RenderWindow* window = Engine::GetRenderWindow();

    MATERIAL_DEFAULT = new Material();
    MATERIAL_DEFAULT->shader = SHADER_BASIC;
    MATERIAL_DEFAULT->texture = TEXTURE_GRASS;
    MATERIAL_DEFAULT->CreateMaterial(window);

    MATERIAL_WALL = new Material();
    MATERIAL_WALL->shader = SHADER_BASIC;
    MATERIAL_WALL->texture = TEXTURE_WALL;
    MATERIAL_WALL->CreateMaterial(window);

    MATERIAL_WATERGUN = new Material();
    MATERIAL_WATERGUN->shader = SHADER_SKYBOX;
    MATERIAL_WATERGUN->texture = TEXTURE_WATERGUN;
    MATERIAL_WATERGUN->CreateMaterial(window);
    
    MATERIAL_DUCK = new Material();
    MATERIAL_DUCK->shader = SHADER_BASIC;
    MATERIAL_DUCK->texture = TEXTURE_DUCK;
    MATERIAL_DUCK->CreateMaterial(window);

    MATERIAL_SKYBOX = new Material();
    MATERIAL_SKYBOX->shader = SHADER_SKYBOX;
    MATERIAL_SKYBOX->texture = TEXTURE_SKYBOX;
    MATERIAL_SKYBOX->CreateMaterial(window);

    MATERIAL_EYE = new Material();
    MATERIAL_EYE->shader = SHADER_BASIC;
    MATERIAL_EYE->texture = TEXTURE_EYE;
    MATERIAL_EYE->CreateMaterial(window);

    MATERIAL_BOUCEPAD = new Material();
    MATERIAL_BOUCEPAD->shader = SHADER_BASIC;
    MATERIAL_BOUCEPAD->texture = TEXTURE_BOUCEPAD;
    MATERIAL_BOUCEPAD->CreateMaterial(window);
    
}
