#pragma once

class DepthStencilBuffer
{
public:
    // DXGI_FORMAT_D32_FLOAT : 4Byte를 깊이 용도로만 활용한다.
    void Init(const WindowInfo& window, DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT);

    D3D12_CPU_DESCRIPTOR_HANDLE	GetDSVCpuHandle() { return _dsvHandle; }
    DXGI_FORMAT GetDSVFormat() { return _dsvFormat; }

private:
    // Depth Stencil View
    ComPtr<ID3D12Resource>              _dsvBuffer; // 화면에 출력 용도가 아니라 연산의 중간과정에만 활용할 것이므로 1개로 충분하다.
    ComPtr<ID3D12DescriptorHeap>        _dsvHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE         _dsvHandle = {};
    DXGI_FORMAT                         _dsvFormat = {};
};

