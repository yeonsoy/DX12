#pragma once
#include "Object.h"

enum class SHADER_TYPE : uint8
{
    DEFERRED,
    FORWARD,
    LIGHTING,
    PARTICLE,
    COMPUTE,
    SHADOW
};

enum class RASTERIZER_TYPE : uint8
{
    CULL_NONE, // 모든 방향 Object를 무시하지 않는다.
    CULL_FRONT, // 시계 방향인 Object 무시
    CULL_BACK, // 반시계 방향인 Object 무시 (Default)
    WIREFRAME, // 안에 있는 모든 내용물이 보이는 것이 아니라 외곽선만 보인다.
};

// Depath Stencil Buffer를 만들어 줄 때 Buffer보다 값이 어떨 때 그려준다.
enum class DEPTH_STENCIL_TYPE : uint8
{
    LESS, // 작을 때 그려준다. (최소값 1) 작거나 같을 땐 그리지 않는다.
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    NO_DEPTH_TEST, // 깊이 테스트(X) + 깊이 기록(O)
    NO_DEPTH_TEST_NO_WRITE, // 깊이 테스트(X) + 깊이 기록(X)
    LESS_NO_WRITE, // 깊이 테스트(O) + 깊이 기록(X)
};

// 결과물을 섞어주는 옵션
enum class BLEND_TYPE : uint8
{
    DEFAULT,
    ALPHA_BLEND,
    ONE_TO_ONE_BLEND,
    END,
};

struct ShaderInfo
{
    SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
    RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
    DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
    BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
    D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

// [일감 기술서] 외주 인력들이 뭘 해야할지 기술

class Shader : public Object
{
public:
    Shader();
    virtual ~Shader();

    void CreateGraphicsShader(const wstring& path, ShaderInfo info = ShaderInfo(), const string& vs = "VS_Main", const string& ps = "PS_Main", const string& gs = "");
    void CreateComputeShader(const wstring& path, const string& name, const string& version);
    void Update();

    SHADER_TYPE GetShaderType() { return _info.shaderType; }

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(D3D_PRIMITIVE_TOPOLOGY topology);

private:
    void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
    void CreateVertexShader(const wstring& path, const string& name, const string& version);
    void CreatePixelShader(const wstring& path, const string& name, const string& version);
    void CreateGeometryShader(const wstring& path, const string& name, const string& version);

private:
    ShaderInfo _info;
    ComPtr<ID3D12PipelineState>			_pipelineState;

    // Graphics Shader
    ComPtr<ID3DBlob>					_vsBlob;
    ComPtr<ID3DBlob>					_psBlob;
    ComPtr<ID3DBlob>					_gsBlob;
    ComPtr<ID3DBlob>					_errBlob;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC  _graphicsPipelineDesc = {};

    // Compute Shader
    ComPtr<ID3DBlob>					_csBlob;
    D3D12_COMPUTE_PIPELINE_STATE_DESC   _computePipelineDesc = {};
};

