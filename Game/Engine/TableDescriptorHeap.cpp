#include "pch.h"
#include "TableDescriptorHeap.h"
#include "Engine.h"

// ************************
// GraphicsDescriptorHeap
// ************************

void GraphicsDescriptorHeap::Init(uint32 count)
{
    _groupCount = count;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = count * (CBV_SRV_REGISTER_COUNT - 1); // b0는 전역
    // Flag는 GPU 메모리에 상주하고, Register에 올려보내야 하므로 VISIBLE, Type은 CBV로 설정.
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _groupSize = _handleSize * (CBV_SRV_REGISTER_COUNT - 1);
}

void GraphicsDescriptorHeap::Clear()
{
    _currentGroupIndex = 0;
}

void GraphicsDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetSRVHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetSRVHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Register로 Descriptor Heap을 보내는 함수
void GraphicsDescriptorHeap::CommitTable()
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    GRAPHICS_CMD_LIST->SetGraphicsRootDescriptorTable(1, handle);

    _currentGroupIndex++; // Register로 데이터를 보낸 후 다음 Table을 가리키도록 이동시킨다.
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetSRVHandle(CBV_REGISTER reg)
{
    return GetSRVHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetSRVHandle(SRV_REGISTER reg)
{
    return GetSRVHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetSRVHandle(uint32 reg)
{
    assert(reg > 0);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    handle.ptr += (reg - 1) * _handleSize;
    return handle;
}

// ************************
// ComputeDescriptorHeap
// ************************

void ComputeDescriptorHeap::Init()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = TOTAL_REGISTER_COUNT; // UAV 포함 Register 개수
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // TODO : 리소스 상태 변경
}

void ComputeDescriptorHeap::CommitTable()
{
    ID3D12DescriptorHeap* descHeap = _descHeap.Get();
    COMPUTE_CMD_LIST->SetDescriptorHeaps(1, &descHeap);

    D3D12_GPU_DESCRIPTOR_HANDLE handle = descHeap->GetGPUDescriptorHandleForHeapStart();
    COMPUTE_CMD_LIST->SetComputeRootDescriptorTable(0, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(CBV_REGISTER reg)
{
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(SRV_REGISTER reg)
{
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(UAV_REGISTER reg)
{
    return GetCPUHandle(static_cast<uint8>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(uint8 reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += reg * _handleSize;
    return handle;
}