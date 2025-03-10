#include "pch.h"
#include "Particules.h"

#include "libs/d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "Utils/CoreUtils.h"
#include "Utils/Debug.h"
#include "Utils/Maths.h"

ParticleShader::ParticleShader(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const std::wstring& vsFile, const std::wstring& psFile, const std::wstring& csFile, const std::string& target, UINT maxParticles)
    : Shader(vsFile, psFile, target), mDevice(device), mCommandList(commandList), mMaxParticleCount(maxParticles)
{
    AddConstantBufferView();
    AddConstantBufferView();
    AddInputElement({"POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0});
    AddInputElement({"VELOCITY", DXGI_FORMAT_R32G32B32A32_FLOAT, 16});
    SetInteriorView(true);
    BuildShader(device, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
    parameterUploader = new UploadBuffer<ParticleParameter>(device, 1, true);
    particlesUploader = new UploadBuffer<ParticleData>(mDevice, mMaxParticleCount, false);
    parameterUploader->Resource()->SetName(L"PARTICLE_PARAM_UPLOADER");
    particlesUploader->Resource()->SetName(L"PARTICLE_UPLOADER");
    InitializeResources();
    InitializePipeline(csFile);
}

ParticleShader::~ParticleShader()
{
    
    mComputePipeline->Release();
    mComputeRootSignature->Release();
    
    if (mParticleBufferIn)
        mParticleBufferIn->Release();
    if (mParticleBufferOut)
        mParticleBufferOut->Release();

    parameterUploader->Resource()->Release();
    particlesUploader->Resource()->Release();
    
}

void ParticleShader::InitializeResources()
{
    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = sizeof(ParticleData) * mMaxParticleCount;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    
    HRESULT hr = mDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&mParticleBufferOut));
    mParticleBufferOut->SetName(L"OUT_PARTICLE_BUFFER");
    
    std::vector<std::uint16_t> indices;
    for (int i = 0; i < mMaxParticleCount; i++)
        indices.push_back(i);

    mParticleIndices = d3dUtils::CreateBuffer(mDevice, mCommandList, indices.data(), sizeof(std::uint16_t) * mMaxParticleCount, IndicesBufferUploader);
    mParticleIndices->SetName(L"PARTICLE_INDEX_BUFFER");

}

void XM_CALLCONV ParticleShader::StartAt(FXMVECTOR direction)
{
    
    std::vector<ParticleData> dataSet(mMaxParticleCount);
    for (int i = 0; i+3 < mMaxParticleCount;)
    {
        float range = 100.0f; // Example range in all directions

        // Generate random offsets within the range
        float posRandX = Maths::RandomFloat(-range, range);
        float posRandY = Maths::RandomFloat(-range, range);
        float posRandZ = Maths::RandomFloat(-range, range);

        float rotationX = Maths::RandomFloat(-0.2f, 0.2f);
        float rotationY = Maths::RandomFloat(-0.2f, 0.2f);
        float rotationZ = Maths::RandomFloat(-0.2f, 0.2f);

        XMVECTOR randomDir = XMVectorScale({posRandX, posRandY, posRandZ}, 0.0003f);
        
        XMStoreFloat4(&dataSet[i].position, XMVectorSet(posRandX+rotationX, posRandY, posRandZ, 1.0f));
        XMStoreFloat4(&dataSet[i].velocity, randomDir);
        XMStoreFloat4(&dataSet[i+1].position, XMVectorSet(posRandX+0.2f, posRandY+rotationY, posRandZ, 1.0f));
        XMStoreFloat4(&dataSet[i+1].velocity, randomDir);
        XMStoreFloat4(&dataSet[i+2].position, XMVectorSet(posRandX+0.2f, posRandY+0.2f, posRandZ+rotationZ, 1.0f));
        XMStoreFloat4(&dataSet[i+2].velocity, randomDir);
        i+=3;
    }

    if (mParticleBufferIn)
        mParticleBufferIn->Release();
    mParticleBufferIn = d3dUtils::CreateBuffer(mDevice, mCommandList, dataSet.data(), sizeof(ParticleData) * mMaxParticleCount, VertexBufferUploader);
}

void ParticleShader::InitializePipeline(std::wstring csFile)
{
    ID3DBlob* computeShader = CompileShader(CoreUtils::GetResourceFolder() + L"shaders/" + csFile, nullptr, "CSMain", "aze.cso", "cs_5_1");

    CD3DX12_ROOT_PARAMETER rootParameters[3];
    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsShaderResourceView(0);
    rootParameters[2].InitAsUnorderedAccessView(0);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(_countof(rootParameters), rootParameters);

    ID3DBlob* serializedRootSig;
    ID3DBlob* errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob);
    Debug::LogifErrored(hr, "Particle root serialize failed");

    // Création de la root signature
    hr = mDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mComputeRootSignature));
    Debug::LogifErrored(hr, "Failed to create particle root signature");

    // Création du pipeline state object (PSO) pour le compute shader
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
    computePSODesc.pRootSignature = mComputeRootSignature;
    computePSODesc.CS = { computeShader->GetBufferPointer(), computeShader->GetBufferSize() };
    
    hr = mDevice->CreateComputePipelineState(&computePSODesc, IID_PPV_ARGS(&mComputePipeline));
    Debug::LogifErrored(hr, "Compute shader failed");
    
}

ID3D12Resource* ParticleShader::GetParticleBufferIn()
{
    return mParticleBufferIn;
}

ID3D12Resource* ParticleShader::GetParticleBufferOut()
{
    return mParticleBufferOut;
}

void ParticleShader::SetParticleBufferIn(ID3D12Resource* buffer)
{
    mParticleBufferIn = buffer;
}

void ParticleShader::SetParticleBufferOut(ID3D12Resource* buffer)
{
    mParticleBufferOut = buffer;
}


ID3D12RootSignature* ParticleShader::GetComputeSignature()
{
    return mComputeRootSignature;
}

UploadBuffer<ParticleParameter>* ParticleShader::GetParticleUploader()
{
    return parameterUploader;
}

int ParticleShader::GetNumberOfParticle()
{
    return mMaxParticleCount;
}

ID3D12PipelineState* ParticleShader::GetComputePipeline()
{
    return mComputePipeline;
}
