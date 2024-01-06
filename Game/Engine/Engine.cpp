#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"

void Engine::Init(const WindowInfo& info)
{
    _window = info;

    // 그려질 화면 크기를 설정
    _viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };

    // CD가 붙어있는 Class의 경우에는 d3dx12.h에 포함된 HelperClass
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    // 전방 선언 후 헤더를 추가하지 않으면 오류 발생. 내부 함수 구조를 알려주지 않았기 때문.
    _device->Init();
    _cmdQueue->Init(_device->GetDevice(), _swapChain);
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
    _rootSignature->Init();
    // drawCall이 너무 늘어나는 경우는 비효율적이므로 주의하는 것이 좋다.
    _tableDescHeap->Init(256);
    _depthStencilBuffer->Init(_window);

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256); // Transform 정보를 넘겨주는 경우가 많다.
    CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

    // 대부분의 객체를 전역으로 사용하다보니 Device가 만들어지지 않은 상태로 호출이 되지 않도록 순서를 모든 것을 init한 이후로 호출하도록 변경한다.
    ResizeWindow(info.width, info.height);

    GET_SINGLE(Input)->Init(info.hwnd);
    GET_SINGLE(Timer)->Init();
    GET_SINGLE(Resources)->Init();
}

void Engine::Update()
{
    GET_SINGLE(Input)->Update();
    GET_SINGLE(Timer)->Update();
    GET_SINGLE(SceneManager)->Update();

    Render();

    ShowFps();
}

void Engine::RenderBegin()
{
    _cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::Render()
{
    RenderBegin();

    GET_SINGLE(SceneManager)->Render();

    RenderEnd();
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

    _depthStencilBuffer->Init(_window);
}

void Engine::ShowFps()
{
    uint32 fps = GET_SINGLE(Timer)->GetFps();

    WCHAR text[100] = L"";
    ::wsprintf(text, L"FPS : %d", fps);

    // 창에 FPS 출력
    ::SetWindowText(_window.hwnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
    uint8 typeInt = static_cast<uint8>(reg);
    assert(_constantBuffers.size() == typeInt);

    shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
    buffer->Init(reg, bufferSize, count);
    _constantBuffers.push_back(buffer);
}
