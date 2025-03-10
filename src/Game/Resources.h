#pragma once
#include "libs/GeometryFactory.h"

struct Material;
struct Texture2D;
class ParticleShader;
struct Shader;

class Resources
{
public:
    Resources() = default;
    ~Resources();
    void LoadCustomMesh();
    void CreateShaders();
    void CreateParticles();
    void CreateTextures();
    void CreateMaterials();
    
    static Resources& instance()
    {
        static Resources instance;
        return instance;
    }
    ////////////////////////////////
    /////// SHADER

    Mesh* MESH_WATER_GUN;
    Mesh* MESH_DUCK;
    Mesh* MESH_SKYBOX;
    Mesh* MESH_EYE;

    ////////////////////////////////
    /////// SHADER

    Shader* SHADER_BASIC;
    Shader* SHADER_SKYBOX;

    ////////////////////////////////
    /////// PARTICLES
    
    ParticleShader* PARTICLE_DEFAULT_PARTICLE ;
    ParticleShader* PARTICLE_WALL_EXPLOSION ;

    ////////////////////////////////
    /////// TEXTURES

    Texture2D* TEXTURE_GRASS;
    Texture2D* TEXTURE_WALL;
    Texture2D* TEXTURE_WATERGUN;
    Texture2D* TEXTURE_DUCK;
    Texture2D* TEXTURE_SKYBOX;
    Texture2D* TEXTURE_EYE;
    Texture2D* TEXTURE_BOUCEPAD;

    ////////////////////////////////
    /////// MATERIALS
    
    Material* MATERIAL_DEFAULT ;
    Material* MATERIAL_WALL ;
    Material* MATERIAL_WATERGUN;
    Material* MATERIAL_SKYBOX;
    Material* MATERIAL_DUCK;
    Material* MATERIAL_EYE;
    Material* MATERIAL_BOUCEPAD;
    
};
