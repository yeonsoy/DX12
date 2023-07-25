#include "pch.h"
#include "SwapChain.h"

void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
    CreateSwapChain(info, dxgi, cmdQueue);
    CreateRTV(device);
}

void SwapChain::Present()
{
    // Present the frame.
    // 현재 화면에 그려준다.
    _swapChain->Present(0, 0);
}

void SwapChain::SwapIndex()
{
    // Index를 Swap한다. 0, 1으로 교환.
    _backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}


void SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
    // 이전에 만든 정보 날린다.
    _swapChain.Reset();

    // 화면에 그려질 종이를 만든다.
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
    sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
    sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
    sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식 (RGB32)
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼
    sd.OutputWindow = info.hwnd;
    sd.Windowed = info.windowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // device가 아닌 dxgi로 생성.
    dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

    for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        _swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtvBuffer[i]));
}

void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
{
    // Descriptor (DX12) = View (~DX11)
    // [서술자 힙]으로 RTV 생성
    // DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
    // CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!
    // 일종의 배열. 같은 종류의 데이터끼리 저장할 수 있다.

    int32 rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

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
        _rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * rtvHeapSize);
        device->CreateRenderTargetView(_rtvBuffer[i].Get(), nullptr, _rtvHandle[i]);
    }
}