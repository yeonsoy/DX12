#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec) // 삼각형을 그리고 싶다면 Vertex를 3개를 받아서 그린다.
{
    _vertexCount = static_cast<uint32>(vec.size());
    uint32 bufferSize = _vertexCount * sizeof(Vertex);

    // GPU에 공간을 할당하도록 요청하는 부분 (UPLOAD 타입 : CPU 쪽에서 GPU를 넘기는 용도로만 활용)
    // 현재는 임시로 공용으로 활용. MS 공식 문서에는 DEFAULT 타입을 공용으로 활용하도록 추천하고 있다.
    // 움직이지 않는 물체 (나무, 돌) 등은 DEFAULT를 사용하는 것이 조금 더 효율적이다.
    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_vertexBuffer)); // VertexBuffer는 원격으로 GPU안에 있는 공간을 가리킨다.

    // Copy the triangle data to the vertex buffer.
    void* vertexDataBuffer = nullptr;
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
    _vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
    ::memcpy(vertexDataBuffer, &vec[0], bufferSize); // 잠시나마 GPU에 CPU 데이터를 복사(Map)할 수 있도록 공간을 빌려준다.
    _vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
    _vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::Render()
{
    // CommandQueue의 RenderBegin과 RenderEnd 사이에서 호출된다.
    CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)

    // 1) Buffer에다가 데이터 세팅
    // 2) Buffer의 주소를 register에다가 전송
    GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
    GEngine->GetCB()->PushData(1, &_transform, sizeof(_transform));

    CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}