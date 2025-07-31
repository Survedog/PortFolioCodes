#ifndef _UTILS_
#define _UTILS_

#pragma enable_d3d11_debug_symbols

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Range [0, 1] to [-1, 1]
    normalMapSample = (2.f * normalMapSample) - 1.f;
    
    // Gram-Schmictt process.
    float3 orthonormalT = normalize(tangentW - dot(tangentW, unitNormalW) * unitNormalW);
    float3 binormal = normalize(cross(unitNormalW, orthonormalT));
        
    return normalize(mul(normalMapSample, float3x3(orthonormalT, binormal, unitNormalW))) ;
}

float3 BilinearInterpolate(float3 v1, float3 v2, float3 v3, float3 v4, float2 uv)
{
	return lerp(lerp(v1, v2, uv.x), lerp(v3, v4, uv.x), uv.y);
}

float2 BilinearInterpolate(float2 v1, float2 v2, float2 v3, float2 v4, float2 uv)
{
	return lerp(lerp(v1, v2, uv.x), lerp(v3, v4, uv.x), uv.y);
}

float BilinearInterpolate(float f1, float f2, float f3, float f4, float2 uv)
{
	return lerp(lerp(f1, f2, uv.x), lerp(f3, f4, uv.x), uv.y);
}

float3 BarycentricInterpolate(float3 v1, float3 v2, float3 v3, float3 uvw)
{
	return v1 * uvw.x + v2 * uvw.y + v3 * uvw.z;
}

float2 BarycentricInterpolate(float2 v1, float2 v2, float2 v3, float3 uvw)
{
	return v1 * uvw.x + v2 * uvw.y + v3 * uvw.z;
}

float3 GetCameraWorldPosition(matrix viewInverseMatrix)
{
	return viewInverseMatrix._41_42_43;
}

#endif // _UTILS_