#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "params.fx"

// unordered access view (Compute Shader 전용)
// Read-Write Texture2D
// 일반적으로는 Read-Only
RWTexture2D<float4> g_rwtex_0 : register(u0);

// 쓰레드 그룹당 쓰레드 개수 (numthread)
// max : 1024 (CS_5.0)
// - 하나의 쓰레드 그룹은 하나의 다중처리기에서 실행
// x, y, z에 대해서 x * y * z가 총 일감의 개수
// 알기 쉬운 좌표로 지정해야 다루기 편하다. (현재는 ID 자체를 좌표로 활용하기 위해서 아래와 같이 지정)
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.y % 2 == 0)
        g_rwtex_0[threadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    else
        g_rwtex_0[threadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
}

#endif