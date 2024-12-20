﻿#pragma once

// ************************
// GraphicsDescriptorHeap
// ************************

class GraphicsDescriptorHeap
{
public:
    void Init(uint32 count);

    void Clear();
    void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
    void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
    void CommitTable();

    ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(CBV_REGISTER reg);
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(SRV_REGISTER reg);

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle(uint32 reg);

private:

    ComPtr<ID3D12DescriptorHeap> _descHeap;
    uint64					_handleSize = 0; // handle 별로 다음 칸까지 필요한 크기
    uint64					_groupSize = 0; // Descriptor Heap Group의 크기
    uint64					_groupCount = 0; // Descriptor Heap Group의 개수

    uint32					_currentGroupIndex = 0; // 일종의 커서. 현재 위치를 가리킨다.
};

// ************************
// ComputeDescriptorHeap
// ************************

class ComputeDescriptorHeap
{
public:
    void Init();

    void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
    void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
    void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

    void CommitTable();

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UAV_REGISTER reg);

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:

    ComPtr<ID3D12DescriptorHeap> _descHeap;
    uint64						_handleSize = 0;
};
