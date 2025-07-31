#ifndef _LIGHTING_UTILS_
#define _LIGHTING_UTILS_

#pragma enable_d3d11_debug_symbols

#include "params.hlsl"

float CalcDistAttenuation(float dist, float fallOffStart, float fallOffEnd)
{
    if (dist <= fallOffStart) return 0.f;
    else if (dist >= fallOffEnd) return 1.f;
    else return saturate((fallOffEnd - dist) / (fallOffEnd - fallOffStart));
}

float CalcSpotAttenuation(float cosTheta, float s)
{
    return pow(max(cosTheta, 0.f), s);
}

float3 SchlickFresnel(float3 R0, float3 normal, float3 toLight)
{
    float f0 = 1.f - saturate(dot(normal, toLight));
    return R0 + (1.f - R0) * (f0 * f0 * f0 * f0 * f0);
}

float3 Phong(float3 normal, float3 toLight, float3 toEye, float3 lightColor, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    float3 reflectVec = reflect(-toLight, normal);
    
	const float shininess = saturate(1.f - materialColor.roughness);
    const float m = shininess * 256.f;
    const float cosNR = max(dot(reflectVec, toEye), 0.f);
    
    const float roughnessFactor = ((m + 8.f) / 8.f) * pow(cosNR, m);
    const float3 fresnelFactor = SchlickFresnel(materialColor.specular, normal, toLight);
    const float lambertFactor = max(dot(toLight, normal), 0.f);
    
    float3 specularLight = lightColor * lambertFactor * fresnelFactor * roughnessFactor * shadowFactor;
    float3 diffuseLight = (lightColor * lambertFactor * shadowFactor + ambientLightColor) * materialColor.diffuseAlbedo.rgb;
    return specularLight + diffuseLight;
}

float3 BlinnPhong(float3 normal, float3 toLight, float3 toEye, float3 lightColor, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    float3 halfVec = normalize(toLight + toEye);
       
	const float shininess = saturate(1.f - materialColor.roughness);
    const float m = shininess * 256.f;
    const float cosNH = max(dot(halfVec, normal), 0.f);
    
    const float roughnessFactor = ((m + 8.f) / 8.f) * pow(cosNH, m);
    const float3 fresnelFactor = SchlickFresnel(materialColor.specular, halfVec, toLight);
    const float lambertFactor = max(dot(toLight, normal), 0.f);
    
    float3 specularLight = lightColor * lambertFactor * fresnelFactor * roughnessFactor * shadowFactor;
    float3 diffuseLight = (lightColor * lambertFactor * shadowFactor + ambientLightColor) * materialColor.diffuseAlbedo.rgb;
    return specularLight + diffuseLight;
}

float3 ComputeDirectionalLight(float3 pos, float3 normal, LightInfo light, float3 eyePos, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    float3 toLight = -light.direction;
    float3 toEye = normalize(eyePos - pos);
    return BlinnPhong(normal, toLight, toEye, light.color, ambientLightColor, materialColor, shadowFactor);
}

float3 ComputePointLight(float3 pos, float3 normal, LightInfo light, float3 eyePos, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    const float fallOffStart = light.range * light.fallOffStartRatio;
    const float fallOffEnd = light.range * light.fallOffEndRatio;
    
    float3 toLight = light.position - pos;
    float distToLight = length(toLight);
    if (distToLight >= fallOffEnd) return 0.f;    
    
    toLight /= distToLight;
    float3 toEye = normalize(eyePos - pos);
            
    light.color *= CalcDistAttenuation(distToLight, fallOffStart, fallOffEnd);
    return BlinnPhong(normal, toLight, toEye, light.color, ambientLightColor, materialColor, shadowFactor);
}

float3 ComputeSpotLight(float3 pos, float3 normal, LightInfo light, float3 eyePos, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    const float fallOffStart = light.range * light.fallOffStartRatio;
    const float fallOffEnd = light.range * light.fallOffEndRatio;
    
    float3 toLight = light.position - pos;
    float distToLight = length(toLight);
    if (distToLight >= fallOffEnd) return 0.f;
    
    toLight /= distToLight;
    float3 toEye = normalize(eyePos - pos);
    
    const float halfAngle = light.angle / 2.f;
    const float cosTheta = dot(-toLight, light.direction);
    if (cosTheta <= cos(halfAngle)) return 0.f;
        
    light.color *= CalcDistAttenuation(distToLight, fallOffStart, fallOffEnd) * CalcSpotAttenuation(cosTheta, 8.f);
    return BlinnPhong(normal, toLight, toEye, light.color, ambientLightColor, materialColor, shadowFactor);
}

float3 ComputeLights(LightInfo lights[MAX_LIGHTS], int lightCount, float3 pos, float3 normal, float3 eyePos, float3 ambientLightColor, MaterialColor materialColor, float shadowFactor = 1.f)
{
    float3 result = 0.f;
   
    [unroll(MAX_LIGHTS)]
    for (int i = 0; i < lightCount; ++i)
    {
        if (lights[i].lightType == 0) // DIRECTIONAL_LIGHT
        {
            result += ComputeDirectionalLight(pos, normal, lights[i], eyePos, ambientLightColor, materialColor, shadowFactor); // For now, only directional light will make shadow.
        }
        else if (lights[i].lightType == 1) // POINT_LIGHT
        {
            result += ComputePointLight(pos, normal, lights[i], eyePos, ambientLightColor, materialColor);
        }
        else if (lights[i].lightType == 2)// SPOT_LIGHT
        {
            result += ComputeSpotLight(pos, normal, lights[i], eyePos, ambientLightColor, materialColor);
        }
    }
    return result;
}

#endif // _LIGHTING_UTILS_