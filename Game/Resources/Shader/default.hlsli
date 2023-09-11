#ifndef _DEFAULT_HLSLI_
#define _DEFAULT_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
};

// 정점 변화를 정의하는 단계
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // 어떤 정점이건 WVP 행렬 적용.
    // float4에 1.f를 넣어주는 것은 좌표의 개념을 만들기 위해서이고,
    // 방향성만 추출하고 싶다면 0.f로 확장한다.
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    // 방향 벡터는 마지막을 0으로 지정해줘야 translation이 적용되지 않는다.
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

// 색상을 정의하는 단계
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);

    LightColor totalColor = (LightColor)0.f;

    // 모든 Light를 순회하면서 Color를 계산한다.
    for (int i = 0; i < g_lightCount; ++i)
    {
        // 몇 번째 광원인지
        LightColor color = CalculateLightColor(i, input.viewNormal, input.viewPos);
        totalColor.diffuse += color.diffuse;
        totalColor.ambient += color.ambient;
        totalColor.specular += color.specular;
    }

    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz; // 물체와 무관하게 빛 자체가 번져 보이는 현상이므로 단순히 +만 해준다.

     return color;
}

#endif
