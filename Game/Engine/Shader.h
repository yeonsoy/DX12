#pragma once
#include "Object.h"

enum class RASTERIZER_TYPE
{
    CULL_NONE, // 모든 방향 Object를 무시하지 않는다.
    CULL_FRONT, // 시계 방향인 Object 무시
    CULL_BACK, // 반시계 방향인 Object 무시 (Default)
    WIREFRAME, // 안에 있는 모든 내용물이 보이는 것이 아니라 외곽선만 보인다.
};

// Depath Stencil Buffer를 만들어 줄 때 Buffer보다 값이 어떨 때 그려준다.
enum class DEPTH_STENCIL_TYPE
{
    LESS, // 작을 때 그려준다. (최소값 1) 작거나 같을 땐 그리지 않는다.
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
};

struct ShaderInfo
{
    RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
    DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
};

// [일감 기술서] 외주 인력들이 뭘 해야할지 기술

class Shader : public Object
{
public:
    Shader();
    virtual ~Shader();

    void Init(const wstring& path, ShaderInfo info = ShaderInfo());
    void Update();

private:
    void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
    void CreateVertexShader(const wstring& path, const string& name, const string& version);
    void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
    ComPtr<ID3DBlob>					_vsBlob;
    ComPtr<ID3DBlob>					_psBlob;
    ComPtr<ID3DBlob>					_errBlob;

    ComPtr<ID3D12PipelineState>			_pipelineState;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};
};

