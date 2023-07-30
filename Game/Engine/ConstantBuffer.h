#pragma once

class ConstantBuffer
{
public:
    ConstantBuffer();
    ~ConstantBuffer();

    void Init(uint32 size, uint32 count);

    void Clear();
    void PushData(int32 rootParamIndex, void* buffer, uint32 size);

    D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);

private:
    void CreateBuffer();

private:
    ComPtr<ID3D12Resource>  _cbvBuffer; // GPU쪽 데이터 (Buffer 하나를 의미하는 것이 아니라 Buffer의 모임 자체)
    BYTE*                   _mappedBuffer = nullptr; // CPU쪽에서 데이터를 복사해오기 위한 Buffer
    uint32                  _elementSize = 0; // 각 요소의 크기
    uint32                  _elementCount = 0; // 각 요소의 개수

    uint32                  _currentIndex = 0; // 사용하고 있는 영역을 제외하고 다음 영역을 가리키도록 만든다.
};
