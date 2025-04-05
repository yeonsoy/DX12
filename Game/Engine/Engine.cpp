#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"

void Engine::Init(const WindowInfo& info)
{
    _window = info;

    // 그려질 화면 크기를 설정
    _viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };

    // CD가 붙어있는 Class의 경우에는 d3dx12.h에 포함된 HelperClass
    _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    // 전방 선언 후 헤더를 추가하지 않으면 오류 발생. 내부 함수 구조를 알려주지 않았기 때문.
    _device->Init();
    _graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
    _computeCmdQueue->Init(_device->GetDevice());
    _swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
    _rootSignature->Init();
    // drawCall이 너무 늘어나는 경우는 비효율적이므로 주의하는 것이 좋다.
    _graphicsDescHeap->Init(256);
    _computeDescHeap->Init();

    CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
    CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256); // Transform 정보를 넘겨주는 경우가 많다.
    CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

    CreateRenderTargetGroups();

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
    GET_SINGLE(InstancingManager)->ClearBuffer();

    Render();

    ShowFps();
}

void Engine::RenderBegin()
{
    _graphicsCmdQueue->RenderBegin();
}

void Engine::Render()
{
    RenderBegin();

    GET_SINGLE(SceneManager)->Render();

    RenderEnd();
}

void Engine::RenderEnd()
{
    _graphicsCmdQueue->RenderEnd();
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

void Engine::CreateRenderTargetGroups()
{
    // DepthStencil
    shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil",
        DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    // SwapChain Group
    {
        vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

        for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
        {
            wstring name = L"SwapChainTarget_" + std::to_wstring(i);

            ComPtr<ID3D12Resource> resource;
            _swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
            rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
        }

        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
    }

    // Shadow Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

        // DXGI_FORMAT_R32_FLOAT (Red 채널만 사용)
        rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"ShadowTarget",
            DXGI_FORMAT_R32_FLOAT, 4096, 4096,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        shared_ptr<Texture> shadowDepthTexture = GET_SINGLE(Resources)->CreateTexture(L"ShadowDepthStencil",
            DXGI_FORMAT_D32_FLOAT, 4096, 4096,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
    }

    // Deferred Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget",
            DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget",
            DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget",
            DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
    }

    // Lighting Group
    {
        vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

        rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget",
            DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget",
            DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
        _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
    }
}