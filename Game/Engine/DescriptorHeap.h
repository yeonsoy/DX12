#pragma once

// [기안서]
// 외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데, 아무 형태로나 요청하면 못 알아먹는다
// - 각종 리소스를 어떤 용도로 사용하는지 꼼꼼하게 적어서 넘겨주는 용도

// DX11 까지는 View라는 용어로 존재.
// GPU에 뭔가 요청할 때 리소스는 무엇인지를 적은 정보를 넘겨주는 것.

class DescriptorHeap // (=View)
{
public:
    void Init(ComPtr<ID3D12Device> device, shared_ptr<class SwapChain> swapChain);

    D3D12_CPU_DESCRIPTOR_HANDLE	    GetRTV(int32 idx) { return _rtvHandle[idx]; }
    D3D12_CPU_DESCRIPTOR_HANDLE	    GetBackBufferView();

private:
    ComPtr<ID3D12DescriptorHeap>    _rtvHeap;
    uint32						    _rtvHeapSize = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE	    _rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

    shared_ptr<class SwapChain>     _swapChain;
};