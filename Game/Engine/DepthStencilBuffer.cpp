#include "pch.h"
#include "DepthStencilBuffer.h"
#include "Engine.h"

// 화면의 픽셀 단위로 내가 픽셀에 그린 물체의 깊이(가장 가까이 있는 물체)를 기록한다.
// 따라서 화면 크기를 알기 위해 window를 넘겨준다.
void DepthStencilBuffer::Init(const WindowInfo& window, DXGI_FORMAT dsvFormat)
{
    _dsvFormat = dsvFormat;

    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(_dsvFormat, window.width, window.height);
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // 매 프레임마다 Depth Buffer를 초기화(1.0 = 아무 물체도 그리지 않은 상태)해준다.
    D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(_dsvFormat, 1.0f, 0);

    DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,     // 초기화할 값을 세팅해주면 최적화에 도움이 된다.
        IID_PPV_ARGS(&_dsvBuffer));

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // DSV 타입의 Descriptor Heap을 생성.

    DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_dsvHeap));

    _dsvHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
    DEVICE->CreateDepthStencilView(_dsvBuffer.Get(), nullptr, _dsvHandle);
}
