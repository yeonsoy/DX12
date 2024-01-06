#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.f;

    float3 viewLightDir = (float3)0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;

    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal)); // saturate를 사용하면 값을 0 ~ 1사이로 보정.
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        // LightPos가 고정값이 아니므로 광원의 위치(View Space)를 구해준다.
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        // 광원과 그려주고 있는 Pixel까지의 거리
        float dist = distance(viewPos, viewLightPos);
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f; // 빛이 닿지 않는 경우
        else
            // 거리가 멀어질수록 빛이 약해질 수 있도록 계산
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2; // 최대 시야각에서 /2한 값

            float3 viewLightVec = viewPos - viewLightPos;
            // 빛이 원래 나가는 방향의 단위벡터
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            // 음수로 나올 수도 있는 값 (cos 값)
            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(viewLightDir, viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    // specular 계산
    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));
    float3 eyeDir = normalize(viewPos); // View Space이므로 카메라의 위치가 (0, 0, 0) = View Position과 동일.
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2); // 영역을 좁히기 위해서 제곱승

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
}

#endif
