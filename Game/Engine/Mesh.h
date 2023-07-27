#pragma once

// 정점으로 이루어진 물체 (정점의 모임)

class Mesh
{
public:
    void Init(vector<Vertex>& vec);
    void Render();

private:
    ComPtr<ID3D12Resource>		_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {}; // Resource를 지칭하고 가리키고 설명하고 있는 것 (=View)
    uint32 _vertexCount = 0;
};

