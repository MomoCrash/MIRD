#include "pch.h"
#include "Material.h"

#include "DDSTextureLoader.h"
#include "RenderWindow.h"
#include "Engine/Engine.h"
#include "Utils/CoreUtils.h"
#include "Utils/Debug.h"

bool Texture2D::CreateTexture2D(RenderWindow* render)
{

    if (IsCreated) return false;
    std::wstring file = CoreUtils::GetResourceFolder() + L"textures/" + filename;
    HRESULT hr = DirectX::CreateDDSTextureFromFile12(
        render->GetDevice(),                // ID3D12Device
        render->GetCommandList(),           // ID3D12GraphicsCommandList
        file.c_str(),       // Chemin du fichier .dds
        &textureResource,        // Ressource de texture créée
        &uploadResource             // Tas de téléchargement (upload heap)
    );

    if (FAILED(hr))
    {
        Debug::Error("Failed to create " + name + " setting default");
        return false;
    }

    textureResource->SetName(L"TEXTURE2D");
    uploadResource->SetName(L"UPLOAD_TEXTURE2D");

    IsCreated = true;
    return true;
}

Material::~Material(){}

Texture2D::~Texture2D()
{
    textureResource->Release();
    uploadResource->Release();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::TextureView()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureResource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    return srvDesc;
}

bool Material::CreateMaterial(RenderWindow* render)
{
    if (shader != nullptr)
    {
        if (shader->BuildShader(render->GetDevice(), render->GetBackBufferFormat(), render->GetDepthStencilFormat()) == false)
            shader = Engine::GetRenderWindow()->GetDefaultShader();
    } else return false;
    
    if (texture->CreateTexture2D(render) != false) {
        texture->textureResource->SetName(L"TEXTURE_RESOURCE");
        texture->textureResource->SetName(L"TEXTURE_UPLOAD_RESOURCE");
        render->AddTexture(texture);
    }
    
    return true;
}
