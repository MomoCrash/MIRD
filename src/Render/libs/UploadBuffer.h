#pragma once

template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer);

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer();

    ID3D12Resource* Resource()const
    {
        return mUploadBuffer;
    }

    void CopyData(int elementIndex, const T& data)
    {
        memcpy(&mMappedData[elementIndex*mElementByteSize], &data, sizeof(T));
    }

private:
    ID3D12Resource* mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};

template <typename T>
UploadBuffer<T>::UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : mIsConstantBuffer(isConstantBuffer)
{
    mElementByteSize = sizeof(T);

    // Constant buffer elements need to be multiples of 256 bytes.
    if(isConstantBuffer)
        mElementByteSize = (sizeof(T) + 255) & ~255;

    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize*elementCount);
    device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mUploadBuffer));

    mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
}

template <typename T>
UploadBuffer<T>::~UploadBuffer()
{
    if(mUploadBuffer != nullptr)
        mUploadBuffer->Unmap(0, nullptr);

    mMappedData = nullptr;
}
