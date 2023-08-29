
cbuffer TRANSFORM_PARAMS : register(b0)
{
    // DX에서의 행렬 접근 순서(row)와 shader의 행렬 접근 순서(column)가 다르다.
    // DX에서의 행렬 접근 순서와 맞춰주기 위해서 row_major 키워드를 사용한다.
    row_major matrix matWVP;
};

cbuffer MATERIAL_PARAMS : register(b1)
{
    int int_0;
    int int_1;
    int int_2;
    int int_3;
    int int_4;
    float float_0;
    float float_1;
    float float_2;
    float float_3;
    float float_4;
};

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);
Texture2D tex_4 : register(t4);

SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// 정점 변화를 정의하는 단계
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // 어떤 정점이건 WVP 행렬 적용.
    // float4에 1.f를 넣어주는 것은 좌표의 개념을 만들기 위해서이고,
    // 방향성만 추출하고 싶다면 0.f로 확장한다.
    output.pos = mul(float4(input.pos, 1.f), matWVP);
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

// 색상을 정의하는 단계
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);
    return color;
}