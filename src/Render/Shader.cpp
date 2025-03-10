#include "pch.h"
#include "Shader.h"

#include "Utils/CoreUtils.h"
#include "Utils/Maths.h"

Shader::Shader(const std::wstring& vsPath, const std::wstring& psPath, const std::string& targetFile) : mIsShaderCreated(false), mTableCount(0), mBufferViewCount(0), mNextParameter(0)
{
    mVertexShader = CompileShader(CoreUtils::GetResourceFolder() + L"shaders/" + vsPath, nullptr, "VS", targetFile, "vs_5_1");
    mPixelShader = CompileShader(CoreUtils::GetResourceFolder() + L"shaders/" + psPath, nullptr, "PS", targetFile, "ps_5_1");
}

Shader::~Shader()
{
    mPSO->Release();
    mVertexShader->Release();
    mPixelShader->Release();
}

void Shader::AddInputElement(Layout layout)
{
    mInputLayout.push_back({ layout.name, 0, layout.format, 0, layout.offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
}

bool Shader::BuildShader(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
    if (mIsShaderCreated) return true;

    auto staticSamplers = GetSamplers();
    
    std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(mNextParameter);
    int index = 0;
    for (int i = 0; i < mTableCount; i++)
    {
        CD3DX12_DESCRIPTOR_RANGE* texTable = new CD3DX12_DESCRIPTOR_RANGE();
        texTable->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
        slotRootParameter[index].InitAsDescriptorTable(1, texTable, D3D12_SHADER_VISIBILITY_PIXEL);
        index++;
    }
    for (int i = 0; i < mBufferViewCount; i++)
    {
        slotRootParameter[index].InitAsConstantBufferView(i);
        index++;
    }

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(mNextParameter,
                                            slotRootParameter.data(),
                                            staticSamplers.size(),
                                            staticSamplers.data(), 
                                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    
    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::string errorMessage((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            std::cout << "Serialization failed: " << errorMessage << std::endl;
        }
        else
        {
            std::cout << "Serialization failed with unknown error" << std::endl;
        }
    }

    device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)
    );

    if(errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }

    if (FAILED(hr)) std::cout << "Signature creation failed" << std::endl;
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.pRootSignature = mRootSignature;
    psoDesc.InputLayout = { GetInputLayout().data(), (UINT)GetInputLayout().size() };
    psoDesc.VS = 
    {
        reinterpret_cast<BYTE*>(GetVertexShader()->GetBufferPointer()), 
        GetVertexShader()->GetBufferSize() 
    };
    psoDesc.PS = 
    { 
        reinterpret_cast<BYTE*>(GetPixelShader()->GetBufferPointer()), 
        GetPixelShader()->GetBufferSize() 
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    if (mIsRenderInterior)
    {
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    }
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    if (mIsWireframe)
    {
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = rtvFormat;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = dsvFormat;
    HRESULT result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO));

    if (FAILED(result))
    {
        std::cerr << "Failed to create render pipeline for shader !\n";
        return false;
    }
    mIsShaderCreated = true;
    
    return true;
}

void Shader::SetWireframe(bool wireframe)
{
    mIsWireframe = wireframe;
}

void Shader::SetInteriorView(bool interior)
{
    mIsRenderInterior = interior;
}

ID3D12PipelineState* Shader::GetRenderPipeline()
{
    return mPSO;
}

ID3D12RootSignature* Shader::GetRootSignature()
{
    return mRootSignature;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Shader::GetSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return { 
		pointWrap, pointClamp,
		linearWrap, linearClamp, 
		anisotropicWrap, anisotropicClamp };
}

ID3DBlob* Shader::GetVertexShader()
{
    return mVertexShader;
}

ID3DBlob* Shader::GetPixelShader()
{
    return mPixelShader;
}

std::vector<D3D12_INPUT_ELEMENT_DESC>& Shader::GetInputLayout()
{
    return mInputLayout;
}

void Shader::AddTextureView()
{
    mTableCount++;
    mNextParameter++;
}

void Shader::AddConstantBufferView() {
    mBufferViewCount++;
    mNextParameter++;
}

ID3DBlob* Shader::CompileShader(const std::wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const std::string& entrypoint,
    const std::string& targetFile,
    const std::string& target)
{

    ID3DBlob* bytCode = nullptr;
    std::ifstream file(target, std::ios::binary);
    if (file.good())
    {
        // Le fichier .cso existe, on le charge directement
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> bytecode(size);
        file.read(bytecode.data(), size);

        HRESULT hr = D3DCreateBlob(size, &bytCode);
        if (FAILED(hr))
            return nullptr;

        memcpy(bytCode->GetBufferPointer(), bytecode.data(), size);
    }
    
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    
    ID3DBlob* errors;
    HRESULT hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entrypoint.c_str(), target.c_str(), compileFlags, 0, &bytCode, &errors);

    if(errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());

    if (FAILED(hr)) { std::cerr << "Failed to compile shader !\n"; return nullptr; }
    
    std::ofstream outFile(targetFile, std::ios::binary);
    outFile.write((char*)bytCode->GetBufferPointer(), bytCode->GetBufferSize());

    return bytCode;
}
