#pragma once

#include <d3d12.h>

#include "Shader.h"

struct ParticleData
{
    XMFLOAT4 position;
    XMFLOAT4 velocity;
};

struct ParticleParameter
{
    XMFLOAT4X4 viewProj;
    XMFLOAT3 gravity;
    float deltaTime;
};

class ParticleShader : public Shader
{
public:
    ParticleShader(ID3D12Device* device,
        ID3D12GraphicsCommandList* commandList,
        const std::wstring& vsFile,
        const std::wstring& psFile,
        const std::wstring& csFile,
        const std::string& target, UINT maxParticles);
    ~ParticleShader();
    template<typename T>
    void Render(UploadBuffer<T>* data);
    void XM_CALLCONV StartAt(FXMVECTOR direction);
    
    ID3D12Resource* GetParticleBufferIn();
    ID3D12Resource* GetParticleBufferOut();

    void SetParticleBufferIn(ID3D12Resource*);
    void SetParticleBufferOut(ID3D12Resource*);

    ID3D12PipelineState* GetComputePipeline();
    ID3D12RootSignature* GetComputeSignature();

    UploadBuffer<ParticleParameter>* GetParticleUploader();
    
    int GetNumberOfParticle();

private:
    void InitializeResources();

    void InitializePipeline(std::wstring csFile);

    ID3D12PipelineState* mComputePipeline;
    ID3D12RootSignature* mComputeRootSignature;
    
    ID3D12Resource* mParticleBufferIn; // Buffers pour stockage des particules
    ID3D12Resource* mParticleBufferOut; // Buffers pour stockage des particules

    ID3D12Resource* VertexBufferUploader = nullptr;
    ID3D12Resource* IndicesBufferUploader = nullptr;
    
    ID3D12Resource* mParticleIndices; 

    UploadBuffer<ParticleParameter>* parameterUploader;
    UploadBuffer<ParticleData>* particlesUploader;
    
    UINT mMaxParticleCount;

    ID3D12Device* mDevice;
    ID3D12GraphicsCommandList* mCommandList;
};

template <typename T>
void ParticleShader::Render(UploadBuffer<T>* data)
{
    mCommandList->SetGraphicsRootSignature(GetRootSignature());
    mCommandList->SetPipelineState(GetRenderPipeline());

    mCommandList->SetGraphicsRootConstantBufferView(0, parameterUploader->Resource()->GetGPUVirtualAddress());
    mCommandList->SetGraphicsRootConstantBufferView(1, data->Resource()->GetGPUVirtualAddress());
    
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    m_vertexBufferView.BufferLocation = GetParticleBufferOut()->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(ParticleData);
    m_vertexBufferView.SizeInBytes = sizeof(ParticleData) * mMaxParticleCount;

    D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
    mIndexBufferView.BufferLocation = mParticleIndices->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    mIndexBufferView.SizeInBytes = sizeof(std::uint16_t) * mMaxParticleCount;

    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    mCommandList->IASetIndexBuffer(&mIndexBufferView);

    // Dessiner les particules
    mCommandList->DrawInstanced(mMaxParticleCount, 1, 0, 0);
    
}
