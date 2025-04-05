#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    // pos와 clipPos 모두 Clip 좌표계의 Position이지만
    // SV_Position은 Pixel Shader로 넘어가는 순간 Pixel 좌표가 되기 때문에 따로 저장한다.
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.clipPos = output.pos;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 투영 좌표계의 z (depth 값 저장)
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

#endif