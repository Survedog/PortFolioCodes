#ifndef _SHADOW_UTILS_
#define _SHADOW_UTILS_

#pragma enable_d3d11_debug_symbols

#include "params.hlsl"

/*
* Calculate the the darkness of the shadow on the surface. (0 means complete darkness, 1 means no shadow)
* @shadowPosH{n}: Position in nth shadow camera's texture space.
* @viewZ: Depth of the position from the rendering camera's view space. (Same as homogeneous clip space position's w value)
*/
float CalcShadowFactor(float4 shadowPosH1, float4 shadowPosH2, float4 shadowPosH3, float viewZ)
{      	
	// Calculate depth ratio from the rendering camera to choose correct shadow map.
	float a = g_projMatrices[g_currentCameraIdx][2][2] + 0.000001f; // a = f / (f - n) = proj[2][2], Add a small value to avoid division by zero. 
	float b = g_projMatrices[g_currentCameraIdx][3][2]; // b = -nf / (f - n) = proj[3][2]
	float nearZ = -b / a; // n = -b / a
	float farZ = -a * nearZ / (1 - a); // f/(f-n) = a -> f(1 - a) = -an -> f = -an / (1 - a)
	float depthRatio = (viewZ - nearZ) / (farZ - nearZ); // The z ratio of position between camera's nearZ and farZ. (0.0 to 1.0)    

	if (g_shadowCam1_PartitionRange.x <= depthRatio && depthRatio <= g_shadowCam1_PartitionRange.y)
	{
		shadowPosH1 /= shadowPosH1.w; // Divide by w to complete projection.
		float depthFromShadowCam = shadowPosH1.z;
		return g_texture_shadowMap_1.SampleCmpLevelZero(g_comp_sampler_pointBorder, shadowPosH1.xy, depthFromShadowCam).r;
	}
	else if (g_shadowCam2_PartitionRange.x <= depthRatio && depthRatio <= g_shadowCam2_PartitionRange.y)
	{
		shadowPosH2 /= shadowPosH2.w; // Divide by w to complete projection.
		float depthFromShadowCam = shadowPosH2.z;
		return g_texture_shadowMap_2.SampleCmpLevelZero(g_comp_sampler_pointBorder, shadowPosH2.xy, depthFromShadowCam).r;
	}
	else if (g_shadowCam3_PartitionRange.x <= depthRatio && depthRatio <= g_shadowCam3_PartitionRange.y)
	{
		shadowPosH3 /= shadowPosH3.w; // Divide by w to complete projection.
		float depthFromShadowCam = shadowPosH3.z;
		return g_texture_shadowMap_3.SampleCmpLevelZero(g_comp_sampler_pointBorder, shadowPosH3.xy, depthFromShadowCam).r;
	}
	else
	{
		return 1.0f; // No shadow for this position.
	}
}

#endif // _SHADOW_UTILS_