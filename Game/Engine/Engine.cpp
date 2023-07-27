#include "pch.h"
#include "Engine.h"

void Engine::Init(const WindowInfo& info)
{
    _window = info;
    ResizeWindow(info.width, info.height);

    // 그려질 화면 크기를 설정
    _viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };

    // CD가 붙어있는 Class의 경우에는 d3dx12.h에 포함된 HelperClass
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    _device = make_shared<Device>();
    _cmdQueue = make_shared<CommandQueue>();
    _swapChain = make_shared<SwapChain>();
    _rootSignature = make_shared<RootSignature>();

    // 전방 선언 후 헤더를 추가하지 않으면 오류 발생. 내부 함수 구조를 알려주지 않았기 때문.
    _device->Init();
    _cmdQueue->Init(_device->GetDevice(), _swapChain);
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
    _rootSignature->Init(_device->GetDevice());
}

void Engine::Render()
{
    RenderBegin();

    // TODO : 나머지 물체들 그려준다

    RenderEnd();
}

void Engine::RenderBegin()
{
    _cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::RenderEnd()
{
    _cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
    _window.width = width;
    _window.height = height;

    RECT rect = { 0, 0, width, height };
    // :: global namespace에서 함수를 찾아온다는 의미.
    // 표준 라이브러리에서 찾아온다는 의미이기도 해서 ::을 붙이지 않아도 동일하게 동작.
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    // window handle을 사용해서 100, 100 위치에 width, height로 변경해라.
    ::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);
}
