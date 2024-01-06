#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
    if (_cbvBuffer)
    {
        if (_cbvBuffer != nullptr)
            _cbvBuffer->Unmap(0, nullptr);  // Constant Buffer가 소멸될 때(프로그램 종료)는 알아서 Unmap이 된다.

        _cbvBuffer = nullptr;
    }
}

void ConstantBuffer::Init(CBV_REGISTER reg, uint32 size, uint32 count)
{
    _reg = reg;

    // 상수 버퍼는 256 바이트 배수로 만들어야 한다 (원칙)
    // 0 256 512 768
    _elementSize = (size + 255) & ~255; // & ~255는 256의 배수로 내림하여 만들어주는 작업. 반올림을 하기 위해 +255를 더한 뒤 작업한다.
    _elementCount = count;

    CreateBuffer();
    CreateView();
}

void ConstantBuffer::CreateBuffer()
{
    uint32 bufferSize = _elementSize * _elementCount; // 요소의 크기 * 요소의 개수
    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    // GPU에 Buffer의 크기만큼 메모리를 할당해준다.
    DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_cbvBuffer));

    _cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
    // We do not need to unmap until we are done with the resource.  However, we must not write to
    // the resource while it is in use by the GPU (so we must use synchronization techniques).
    // Unmap을 바로 하지 않는 이유는 데이터를 계속 사용해서 프레임마다 데이터를 복사할 것이기 때문이다.
    // mappedBuffer에 데이터를 집어넣은 상태에서 GPU가 데이터를 읽고있는데 또 데이터를 쓰려고 하면 문제가 발생한다.
    // 따라서 fence를 사용하는 등의 동기화가 필요하다.
}

void ConstantBuffer::CreateView()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
    cbvDesc.NumDescriptors = _elementCount;
    // Constant Buffer로 활용할 예정이므로 Flag는 None, Type은 CBV_SRV_UAV
    cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

    // _cbvHeap을 갖고 있으면 아래의 변수들을 저장하지 않더라도 사용할 수 있다.
    _cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
    // GPU 사양마다 IncrementSize가 다르므로 직접 꺼내와야 한다.
    _handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    for (uint32 i = 0; i < _elementCount; ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetSRVHandle(i);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
        cbvDesc.SizeInBytes = _elementSize;   // CB size is required to be 256-byte aligned.

        // cbvHandle에 cbvDesc를 생성하도록 요청.
        DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);
    }
}

void ConstantBuffer::Clear()
{
    _currentIndex = 0;
}

void ConstantBuffer::PushData(void* buffer, uint32 size)
{
    // 데이터를 너무 많이 사용해서 Buffer의 크기를 벗어나는 지 확인
    assert(_currentIndex < _elementCount);
    assert(_elementSize == ((size + 255) & ~255));

    // CPU에서 GPU로 데이터를 복사
    ::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

    // 일감 등록
    // D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
    // CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetSRVHandle(_currentIndex); // 데이터를 넣어준 Handle을 전달
    GEngine->GetTableDescHeap()->SetCBV(cpuHandle, _reg);
    _currentIndex++;
}

void ConstantBuffer::SetGlobalData(void* buffer, uint32 size)
{
    assert(_elementSize == ((size + 255) & ~255));
    ::memcpy(&_mappedBuffer[0], buffer, size);
    CMD_LIST->SetGraphicsRootConstantBufferView(0, GetGpuVirtualAddress(0));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
    D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress(); // 시작주소
    objCBAddress += index * _elementSize;
    return objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetSRVHandle(uint32 index)
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);
}
