#pragma once
#include <d3d12.h>

class RenderWindow;
struct Material;
class Shader;

struct Texture2D
{
    ~Texture2D();
    std::string name;

    std::wstring filename;
    ID3D12Resource* textureResource;
    ID3D12Resource* uploadResource;

    int textureOffset ;

    D3D12_SHADER_RESOURCE_VIEW_DESC TextureView();

private:
    bool CreateTexture2D(RenderWindow* render);

    bool IsCreated;

    friend Material;
};

struct Material
{
    ~Material();
    Shader* shader;
    Texture2D* texture;
    
    bool CreateMaterial(RenderWindow* render);
};
