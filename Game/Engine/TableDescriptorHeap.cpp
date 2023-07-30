#include "pch.h"
#include "TableDescriptorHeap.h"
#include "Engine.h"

void TableDescriptorHeap::Init(uint32 count)
{
    _groupCount = count;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = count * REGISTER_COUNT;
    // Flag는 GPU 메모리에 상주하고, Register에 올려보내야 하므로 VISIBLE, Type은 CBV로 설정.
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

    _handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _groupSize = _handleSize * REGISTER_COUNT;
}

void TableDescriptorHeap::Clear()
{
    _currentGroupIndex = 0;
}

void TableDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

    uint32 destRange = 1;
    uint32 srcRange = 1;
    DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Register로 Descriptor Heap을 보내는 함수
void TableDescriptorHeap::CommitTable()
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    CMD_LIST->SetGraphicsRootDescriptorTable(0, handle);

    _currentGroupIndex++; // Register로 데이터를 보낸 후 다음 Table을 가리키도록 이동시킨다.
}

D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetCPUHandle(CBV_REGISTER reg)
{
    return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetCPUHandle(uint32 reg)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += _currentGroupIndex * _groupSize;
    handle.ptr += reg * _handleSize;
    return handle;
}