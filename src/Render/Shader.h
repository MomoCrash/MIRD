#pragma once

#include "libs/d3dUtils.h"

using namespace DirectX;

struct LightData
{
    XMFLOAT4 position {1.0f, 1.0f, 1.0f, 1.0f};
    XMFLOAT4 color {1.0f, 1.0f, 1.0f, 1.0f};
    float strength = 1.0f;
    XMFLOAT3 padding;
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View;
    DirectX::XMFLOAT4X4 ViewProj;
    DirectX::XMFLOAT4 gAmbientLight;
    DirectX::XMFLOAT4 EyePosW = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    LightData light[32];
    int lightNumber;
    XMINT3 padding;
};

struct ObjectConstants
{
    XMFLOAT4X4 World;
};

struct Layout
{
    LPCSTR name;
    DXGI_FORMAT format;
    UINT offset;
};

struct Shader
{
    Shader(const std::wstring& vsPath, const std::wstring& psPath, const std::string& target);
    ~Shader();
    bool BuildShader(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    
    ID3DBlob* GetVertexShader();
    ID3DBlob* GetPixelShader();

    void AddInputElement(Layout layout);
    std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();

    void AddTextureView();
    void AddConstantBufferView();

    void SetWireframe(bool wireframe);
    void SetInteriorView(bool interior);

    ID3D12PipelineState* GetRenderPipeline();
    ID3D12RootSignature* GetRootSignature();
    
    static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetSamplers();

protected:
    ID3DBlob* CompileShader(
    const std::wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const std::string& entrypoint,
    const std::string& targetFile,
    const std::string& target);
    
private:
    ID3D12PipelineState* mPSO;

    UINT mBufferViewCount;
    UINT mTableCount;
    
    UINT mNextParameter;
    ID3D12RootSignature* mRootSignature;

    ID3DBlob* mVertexShader;
    ID3DBlob* mPixelShader;
    
    bool mIsShaderCreated;
    bool mIsWireframe;
    bool mIsRenderInterior;
    bool mIsUsingPS;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
};