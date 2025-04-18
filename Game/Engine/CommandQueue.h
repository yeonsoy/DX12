﻿#pragma once

class SwapChain;

// ************************
// GraphicsCommandQueue
// ************************

class GraphicsCommandQueue
{
public:
    ~GraphicsCommandQueue();

    void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
    void WaitSync();

    // 실질적으로 게임이 그려지는 부분과 관련
    void RenderBegin();
    void RenderEnd();

    void FlushResourceCommandQueue();

    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }
    ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return	_resCmdList; }

private:
    // CommandQueue : DX12에 등장
    // 외주를 요청할 때, 하나씩 요청하면 비효율적
    // [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
    // 택배를 엄청 많이 보낸다고 했을 때 1개씩 보내기보단 한 번에 보내는 것이 효율적이다.
    // 일감을 밀어넣다가 한꺼번에 GPU에 요청하는 것에 연관된 3가지
    ComPtr<ID3D12CommandQueue>          _cmdQueue;
    ComPtr<ID3D12CommandAllocator>      _cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>   _cmdList;

    ComPtr<ID3D12CommandAllocator>      _resCmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>   _resCmdList;

    // Fence : 울타리(?)
    // 상대방이 작업이 끝날 때까지 기다리겠다는 의미
    // CommandQueue와 연동해서 사용. 특정 일감 (fenceValue)이 끝날 때까지 대기
    // CPU / GPU 동기화를 위한 간단한 도구
    ComPtr<ID3D12Fence>                 _fence;
    uint32                              _fenceValue = 0;
    HANDLE                              _fenceEvent = INVALID_HANDLE_VALUE;

    shared_ptr<SwapChain>         _swapChain;
};

// ************************
// ComputeCommandQueue
// ************************

class ComputeCommandQueue
{
public:
    ~ComputeCommandQueue();

    void Init(ComPtr<ID3D12Device> device);
    void WaitSync();
    void FlushComputeCommandQueue();

    // Resource와 관련된 Command List는 따로 관리하지 않는다.
    ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:
    ComPtr<ID3D12CommandQueue>			_cmdQueue;
    ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
    ComPtr<ID3D12GraphicsCommandList>	_cmdList;

    ComPtr<ID3D12Fence>					_fence;
    uint32								_fenceValue = 0;
    HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};