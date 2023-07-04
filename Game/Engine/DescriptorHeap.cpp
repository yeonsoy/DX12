#include "pch.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
    _swapChain = swapChain;

    // Descriptor (DX12) = View (~DX11)
    // [서술자 힙]으로 RTV 생성
    // DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
    // CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!
    // 일종의 배열. 같은 종류의 데이터끼리 저장할 수 있다.

    _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvDesc.NodeMask = 0;

    // 같은 종류의 데이터끼리 배열로 관리
    // RTV 목록 : [ ] [ ]
    device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

    // 빈 배열을 만들었으므로 배열을 채워준다.
    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        // 시작 주소에서 heapSize만큼 더해주면서 변화
        _rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
        device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
    return GetRTV(_swapChain->GetCurrentBackBufferIndex());
}