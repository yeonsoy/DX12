#pragma once

enum class CONSTANT_BUFFER_TYPE : uint8
{
    GLOBAL,
    TRANSFORM,
    MATERIAL,
    END
};

enum
{
    CONSTANT_BUFFER_COUNT = static_cast<uint8>(CONSTANT_BUFFER_TYPE::END)
};

class ConstantBuffer
{
public:
    ConstantBuffer();
    ~ConstantBuffer();

    void Init(CBV_REGISTER reg, uint32 size, uint32 count);

    void Clear();
    void PushGraphicsData(void* buffer, uint32 size);
    void SetGraphicsGlobalData(void* buffer, uint32 size);
    void PushComputeData(void* buffer, uint32 size);

    D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
    void CreateBuffer();
    void CreateView();

private:
    ComPtr<ID3D12Resource>  _cbvBuffer; // GPU쪽 데이터 (Buffer 하나를 의미하는 것이 아니라 Buffer의 모임 자체)
    BYTE*                   _mappedBuffer = nullptr; // CPU쪽에서 데이터를 복사해오기 위한 Buffer
    uint32                  _elementSize = 0; // 각 요소의 크기
    uint32                  _elementCount = 0; // 각 요소의 개수

    ComPtr<ID3D12DescriptorHeap>    _cbvHeap; // CBV
    D3D12_CPU_DESCRIPTOR_HANDLE     _cpuHandleBegin = {}; // 배열의 시작하는 Handle
    uint32                          _handleIncrementSize = 0; // 다음 요소로 넘어가기 위한 Size

    uint32                  _currentIndex = 0; // 사용하고 있는 영역을 제외하고 다음 영역을 가리키도록 만든다.
    
    CBV_REGISTER            _reg = {};
};
