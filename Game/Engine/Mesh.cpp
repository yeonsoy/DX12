#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{

}

Mesh::~Mesh()
{

}

void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
    CreateVertexBuffer(vertexBuffer);
    CreateIndexBuffer(indexBuffer);
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer) // 삼각형을 그리고 싶다면 Vertex를 3개를 받아서 그린다.
{
    _vertexCount = static_cast<uint32>(buffer.size());
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
    ::memcpy(vertexDataBuffer, &buffer[0], bufferSize); // 잠시나마 GPU에 CPU 데이터를 복사(Map)할 수 있도록 공간을 빌려준다.
    _vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
    _vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
    _indexCount = static_cast<uint32>(buffer.size());
    uint32 bufferSize = _indexCount * sizeof(uint32);

    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_indexBuffer));

    void* indexDataBuffer = nullptr;
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
    _indexBuffer->Map(0, &readRange, &indexDataBuffer);
    ::memcpy(indexDataBuffer, &buffer[0], bufferSize);
    _indexBuffer->Unmap(0, nullptr);

    _indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
    _indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32Bit (4Byte uint32 사용)
    _indexBufferView.SizeInBytes = bufferSize;
}

void Mesh::Render(uint32 instanceCount)
{
    // CommandQueue의 RenderBegin과 RenderEnd 사이에서 호출된다.
    // Input Assmbler에 Vertex Buffer를 넘겨주는 작업
    GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
    GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

    // 1) Buffer에다가 데이터 세팅
    // < Constant를 직접 전달 시 >
    // 2) Buffer의 주소를 register에다가 전송
    // GEngine->GetCB()->PushGraphicsData(0, &_transform, sizeof(_transform));
    // GEngine->GetCB()->PushGraphicsData(1, &_transform, sizeof(_transform));

    // < Table로 전달 시 >
    // 2) TableDescHeap에다가 CBV 전달
    // 3) 모두 세팅이 끝났으면 TableDescHeap 커밋
    GEngine->GetGraphicsDescHeap()->CommitTable();

    // Vetex를 이용해서 그려주는 버전
    // GRAPHICS_CMD_LIST->DrawInstanced(_vertexCount, instanceCount, 0, 0);

    // Index를 이용해서 그려주는 버전
    GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}

void Mesh::Render(shared_ptr<InstancingBuffer>& buffer)
{
    D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };

    // Buffer가 2개가 들어간다.
    GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
    GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

    GEngine->GetGraphicsDescHeap()->CommitTable();

    GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, buffer->GetCount(), 0, 0, 0);
}
