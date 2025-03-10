#include "pch.h"
#include "RenderWindow.h"

#include "libs/UploadBuffer.h"
#include "libs/GeometryFactory.h"
#include "Utils/Maths.h"
#include "DDSTextureLoader.h"
#include "Material.h"
#include "Utils/CoreUtils.h"


RenderWindow::RenderWindow(HINSTANCE instance) : Window(instance),
                                                 mDefaultShader(nullptr),
                                                 mDefaultMaterial(nullptr),
                                                 mView(), mViewPosition(), nextTextureOffset(0), nextLightOffset(0),
                                                 mProj(), mViewProj(),
                                                 mGlobalConstantBuffer(nullptr), mMainPassCB()
{
}

RenderWindow::~RenderWindow()
{
	RenderWindow::EndDraw();
	
	delete mDefaultMaterial->shader;
	delete mDefaultMaterial->texture;

	mSrvDescriptorHeap->Release();

	mGlobalConstantBuffer->Resource()->Release();
}

bool RenderWindow::Initialize()
{
	
	Window::Initialize();

	Maths::Identity4X4(&mView);

	OpenCommandList();
	
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	BuildDescriptorHeaps();
	
	GeometryFactory::instance().Initialize(mDevice, mCommandList);
	
	mGlobalConstantBuffer = new UploadBuffer<PassConstants>(mDevice, 1, true);
	mGlobalConstantBuffer->Resource()->SetName(L"PASS_BUFFER");
	
	mDefaultShader = new Shader(L"default.hlsl", L"default.hlsl", "defaultShader.cso");
	mDefaultShader->AddInputElement({"POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0});

	mDefaultMaterial = new Material();
	mDefaultMaterial->shader = mDefaultShader;
	mDefaultMaterial->texture = new Texture2D();
	mDefaultMaterial->texture->filename = L"texture.dds";
	mDefaultMaterial->texture->name = "default";
	mDefaultMaterial->CreateMaterial(this);

	CloseCommandList();

	return 0;
}

void RenderWindow::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 10;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap));
}

void RenderWindow::Update()
{
	ShowCursor(false);
	UpdateGlobalConstant();
}

void RenderWindow::BeginDraw()
{
	mDirectCmdListAlloc->Reset();
	mCommandList->Reset(mDirectCmdListAlloc, nullptr);
	
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &barrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE currentBackBufferView(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GetDepthStencilView();
	
	mCommandList->ClearRenderTargetView(currentBackBufferView, Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
	mCommandList->OMSetRenderTargets(1,
		&currentBackBufferView, true, &depthStencilView);
	
	ID3D12DescriptorHeap* heaps[] = { mSrvDescriptorHeap };
	mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

	
}

void RenderWindow::EndDraw()
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	mCommandList->ResourceBarrier(1, &barrier);
	
	mCommandList->Close();
	
	ID3D12CommandList* cmdsLists[] = { mCommandList };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	
	mSwapChain->Present(0, 0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
	
	FlushCommandQueue();
}

void RenderWindow::UpdateGlobalConstant()
{
	
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&mViewProj, viewProj);
	
	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	
	XMStoreFloat4(&mMainPassCB.gAmbientLight, { 0.25f, 0.25f, 0.35f, 1.0f });
	mMainPassCB.lightNumber = nextLightOffset;
	mGlobalConstantBuffer->CopyData(0, mMainPassCB);

	// Reset the pass buffer for the next frame
	mMainPassCB = PassConstants();
	nextLightOffset = 0;
}

void RenderWindow::OnResize()
{
    Window::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX ProjMatrix = XMMatrixPerspectiveFovLH(0.25f*Maths::PI, AspectRatio(), 0.1f, 500);
	XMStoreFloat4x4(&mProj, ProjMatrix);
}

void RenderWindow::OnMouseDown(WPARAM btnState, int x, int y)	{}
void RenderWindow::OnMouseUp(WPARAM btnState, int x, int y)		{}
void RenderWindow::OnMouseMove(WPARAM btnState, int x, int y)	{}
void RenderWindow::OnKeyPressed(WPARAM btnState, int x, int y)	{}

void RenderWindow::UpdateCameraConstants(FXMVECTOR eyePosition, FXMMATRIX view)
{
	XMStoreFloat4x4(&mView, XMMatrixInverse(nullptr, view));
	XMStoreFloat3(&mViewPosition, eyePosition);
}

void RenderWindow::AddTexture(Texture2D* texture)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(nextTextureOffset, mCbvSrvUavDescriptorSize);
	D3D12_SHADER_RESOURCE_VIEW_DESC textureView = texture->TextureView();
	mDevice->CreateShaderResourceView(texture->textureResource, &textureView, hDescriptor);
	texture->textureOffset = nextTextureOffset++;
}

Shader* RenderWindow::GetDefaultShader() const
{
	return mDefaultShader;
}

Material* RenderWindow::GetDefaultMaterial() const
{
	return mDefaultMaterial;
}

void RenderWindow::ComputeParticle(ParticleShader* data, float deltaTime)
{
    
	mCommandList->SetComputeRootSignature(data->GetComputeSignature());
	mCommandList->SetPipelineState(data->GetComputePipeline());
	
	ParticleParameter params;
	XMStoreFloat4x4(&params.viewProj, XMMatrixTranspose(XMLoadFloat4x4(&mViewProj)));
	params.gravity = XMFLOAT3(0.0f, -0.081f, 0.0f);
	params.deltaTime = deltaTime;
	data->GetParticleUploader()->CopyData(0, params);
	
	mCommandList->SetComputeRootConstantBufferView(0, data->GetParticleUploader()->Resource()->GetGPUVirtualAddress());
	
	mCommandList->SetComputeRootShaderResourceView(1, data->GetParticleBufferIn()->GetGPUVirtualAddress());
	mCommandList->SetComputeRootUnorderedAccessView(2, data->GetParticleBufferOut()->GetGPUVirtualAddress());
	
	mCommandList->Dispatch((data->GetNumberOfParticle() + 255) / 256, 1, 1);

	auto resTemp = data->GetParticleBufferIn();
	data->SetParticleBufferIn(data->GetParticleBufferOut());
	data->SetParticleBufferOut(resTemp);
	
}

void RenderWindow::ComputeLight(FXMVECTOR position, FXMVECTOR color, float strength)
{
	XMStoreFloat4(&mMainPassCB.light[nextLightOffset].position, position);
	XMStoreFloat4(&mMainPassCB.light[nextLightOffset].color, color);
	mMainPassCB.light[nextLightOffset].strength = strength;
	nextLightOffset++;
}

void RenderWindow::OpenCommandList()
{
	mDirectCmdListAlloc->Reset();
	
	mCommandList->Reset(mDirectCmdListAlloc, nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &barrier);
}

void RenderWindow::CloseCommandList()
{

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->Close();
	ID3D12CommandList* cmdsLists[] = { mCommandList };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	
	FlushCommandQueue();
}
