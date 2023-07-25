#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"

class Engine
{
public:
    void Init(const WindowInfo& info);
    void Render();
    
public:
    shared_ptr<Device> GetDevice() { return _device; }
    shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
    shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }

public:
    void RenderBegin();
    void RenderEnd();
    void ResizeWindow(int32 width, int32 height);

private:
    // 그려질 화면 크기 관련
    WindowInfo      _window;
    D3D12_VIEWPORT  _viewport = {};
    D3D12_RECT      _scissorRect = {};

    // Device : GPU에 접근하거나, 리소스 요청할 때 사용하는 인력사무소 대표 (핵심)
    shared_ptr<Device>            _device; 
    // Command Queue : GPU에게 요청을 보낼 때, 요청할 것이 생길 때마다 보내는 것이 아니라 뭉쳐서 한 번에 보내기 위한 수단 (커맨드 패턴에서 사용하는 큐)
    shared_ptr<CommandQueue>      _cmdQueue; 
    // Swap Chain : Double Buffering과 관련된 부분. 
    // Buffer를 두 개를 두고 하나는 GPU가 그리는 용도, 하나는 현재 화면에서 출력하는 용도로 사용하다가
    // 다음 프레임에서는 역할을 뒤바꾸어서 이전 프레임에서 그리던 Buffer를 화면에 노출하고, 현재 그리던 Buffer를 다시 GPU가 그리는 용도로 활용.
    // 실시간으로 그려지는 단계가 보여지지 않고 (깜빡거리는 현상 없이) 항상 완성된 결과물만 보여준다.
    shared_ptr<SwapChain>         _swapChain; 
    // Descriptor Heap (=View) : 어떤 리소스(Buffer로 활용하려는 리소스)를 서술하는 객체. 기안서. 
};

