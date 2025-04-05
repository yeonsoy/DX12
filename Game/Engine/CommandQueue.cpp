#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

// ************************
// GraphicsCommandQueue
// ************************

GraphicsCommandQueue::~GraphicsCommandQueue()
{
    // fenceEvent를 활용하고 있었다면 종료.
    ::CloseHandle(_fenceEvent);
}

void GraphicsCommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
    _swapChain = swapChain;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    // 직접적으로 만드는 것이 아니라, Com 객체의 어떤 함수를 이용해서 간접적으로 생성.
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

    // - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

    // GPU가 하나인 시스템에서는 0으로
    // DIRECT or BUNDLE
    // Allocator
    // 초기 상태 (그리기 명령은 nullptr 지정)
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

    // CommandList는 Close / Open 상태가 있는데
    // Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
    _cmdList->Close();

    // Resource Load 용 CommandList 생성
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));

    // CreateFence
    // - CPU와 GPU의 동기화 수단으로 쓰인다
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

    // 신호등처럼 파란불이 켜질 때까지 기다린다.
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void GraphicsCommandQueue::WaitSync()
{
    // Advance the fence value to mark commands up to this fence point.
    // fenceValue는 0부터 시작해서 1씩 증가.
    _fenceValue++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    // CommandQueue에 fence를 건네준다.
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    // Wait until the GPU has completed commands up to this fence point.
    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // Fire event when GPU hits current fence.  
        // fenceValue까지 기다린다.
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

        // Wait until the GPU hits current fence event is fired.
        // fenceValue까지 (일감이 끝날 때까지) 완료되면 Event를 보낸다.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void GraphicsCommandQueue::RenderBegin()
{
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    int8 backIndex = _swapChain->GetBackBufferIndex();

    // D3D12_RESOURCE_STATE_PRESENT -> D3D12_RESOURCE_STATE_RENDER_TARGET 로 변경하겠다는 의미
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
        D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
        D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

    // Root Signature를 사용하도록 전달.
    _cmdList->SetGraphicsRootSignature(GRAPHICS_ROOT_SIGNATURE.Get());
  
    GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
    GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

    GEngine->GetGraphicsDescHeap()->Clear();

    ID3D12DescriptorHeap* descHeap = GEngine->GetGraphicsDescHeap()->GetDescriptorHeap().Get();
    // SetDescriptorHeaps은 자주 사용하면 비효율적이므로 매 프레임마다 1번씩만 호출되도록 한다.
    // 해당 함수를 누락하게 되면 SetGraphicsRootDescriptorTable에서 크래시가 발생한다.
    _cmdList->SetDescriptorHeaps(1, &descHeap);

    // 실행 예약.
    _cmdList->ResourceBarrier(1, &barrier);
}

void GraphicsCommandQueue::RenderEnd()
{
    int8 backIndex = _swapChain->GetBackBufferIndex();

    // D3D12_RESOURCE_STATE_RENDER_TARGET -> D3D12_RESOURCE_STATE_PRESENT 로 변경하겠다는 의미
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
        D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();

    // 커맨드 리스트 수행
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    _swapChain->Present();

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    WaitSync();

    _swapChain->SwapIndex();
}

void GraphicsCommandQueue::FlushResourceCommandQueue()
{
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    WaitSync();

    // 재사용 준비
    _resCmdAlloc->Reset();
    _resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}

// ************************
// ComputeCommandQueue
// ************************

ComputeCommandQueue::~ComputeCommandQueue()
{
    ::CloseHandle(_fenceEvent);
}

void ComputeCommandQueue::Init(ComPtr<ID3D12Device> device)
{
    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_cmdQueue));

    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&_cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

    // CreateFence
    // - CPU와 GPU의 동기화 수단으로 쓰인다
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void ComputeCommandQueue::WaitSync()
{
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue)
    {
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void ComputeCommandQueue::FlushComputeCommandQueue()
{
    _cmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    auto t = _countof(cmdListArr);
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    WaitSync();

    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    COMPUTE_CMD_LIST->SetComputeRootSignature(COMPUTE_ROOT_SIGNATURE.Get());
}