#pragma once

#include "libs/d3dUtils.h"
#include "Window.h"
#include "Shader.h"
#include "libs/UploadBuffer.h"
#include "libs/Particules.h"

struct Texture2D;
struct Material;
using namespace DirectX;

class RenderWindow : public Window
{
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    
public:
    RenderWindow(HINSTANCE instance);
    ~RenderWindow();
    
    bool Initialize() override;
    
    void Update() override;
    
    void BeginDraw() override;
    void EndDraw() override;

    void XM_CALLCONV UpdateCameraConstants(FXMVECTOR eyePosition, FXMMATRIX view);
    
    void AddTexture(Texture2D* texture);
    Shader* GetDefaultShader() const;
    Material* GetDefaultMaterial() const;

    template<typename T>
    void Draw(Material* material, Mesh* mesh, UploadBuffer<T>* constantBuffer);
    
    void ComputeParticle(ParticleShader* data, float deltaTime);
    void XM_CALLCONV ComputeLight(FXMVECTOR position, FXMVECTOR color, float strength);

    void OpenCommandList();
    void CloseCommandList();
    
private:
    void OnMouseDown(WPARAM btnState, int x, int y) override;
    void OnMouseUp(WPARAM btnState, int x, int y) override;
    void OnMouseMove(WPARAM btnState, int x, int y) override;
    void OnKeyPressed(WPARAM btnState, int x, int y) override;
    
    void UpdateGlobalConstant(); // Used as global update for global world data
    
    void OnResize() override;
    
    void BuildDescriptorHeaps();
    
    Shader* mDefaultShader;
    Material* mDefaultMaterial;

    ID3D12DescriptorHeap* mSrvDescriptorHeap = nullptr;

    UploadBuffer<PassConstants>* mGlobalConstantBuffer;
    
    PassConstants mMainPassCB;
    XMFLOAT4X4 mView;
    XMFLOAT4X4 mProj;
    XMFLOAT4X4 mViewProj;
    XMFLOAT3 mViewPosition;
    
    int nextTextureOffset;
    int nextLightOffset;
};

#include "Material.h"

// TODO Mettre le call
template <typename T>
void RenderWindow::Draw(Material* material, Mesh* mesh, UploadBuffer<T>* constantBuffer)
{

    mCommandList->SetGraphicsRootSignature(material->shader->GetRootSignature());
    mCommandList->SetGraphicsRootConstantBufferView(2, mGlobalConstantBuffer->Resource()->GetGPUVirtualAddress());
    mCommandList->SetPipelineState(material->shader->GetRenderPipeline());

    D3D12_VERTEX_BUFFER_VIEW vertexBuffer = mesh->VertexBufferView();
    D3D12_INDEX_BUFFER_VIEW indexBuffer = mesh->IndexBufferView();

    mCommandList->IASetVertexBuffers(0, 1, &vertexBuffer);
    mCommandList->IASetIndexBuffer(&indexBuffer);
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    tex.Offset(material->texture->textureOffset, mCbvSrvUavDescriptorSize);

    mCommandList->SetGraphicsRootDescriptorTable(0, tex);
    mCommandList->SetGraphicsRootConstantBufferView(1, constantBuffer->Resource()->GetGPUVirtualAddress());

    mCommandList->DrawIndexedInstanced(mesh->MeshData.Indices32.size(), 1, 0, 0, 0);
}