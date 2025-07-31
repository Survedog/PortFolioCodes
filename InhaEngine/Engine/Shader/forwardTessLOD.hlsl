#ifndef _FORWARD_TESS_LOD_
#define _FORWARD_TESS_LOD_

#pragma enable_d3d11_debug_symbols

#include "lightingUtils.hlsl"
#include "shadowUtils.hlsl"
#include "params.hlsl"
#include "utils.hlsl"
#include "tessellation_PNTriangle.hlsl"

VertexOut VS(VertexIn vin)
{
#ifdef SKINNED
	if (vin.BoneIndices.x < MAX_SKELETON_BONES)
	{
		vin.posL = mul(float4(vin.posL, 1.f), g_boneMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
		vin.tangentL = mul(float4(vin.tangentL, 1.f), g_boneMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
		vin.normalL = mul(float4(vin.tangentL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
	}

	if (vin.BoneIndices.y < MAX_SKELETON_BONES)
	{
		vin.posL = mul(float4(vin.posL, 1.f), g_boneMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
		vin.tangentL = mul(float4(vin.tangentL, 1.f), g_boneMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
		vin.normalL = mul(float4(vin.tangentL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
	}

	if (vin.BoneIndices.z < MAX_SKELETON_BONES)
	{
		vin.posL = mul(float4(vin.posL, 1.f), g_boneMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
		vin.tangentL = mul(float4(vin.tangentL, 1.f), g_boneMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
		vin.normalL = mul(float4(vin.tangentL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
	}

	if (vin.BoneIndices.w < MAX_SKELETON_BONES)
	{
		float BoneWeightW = 1 - (vin.BoneWeights.x + vin.BoneWeights.y + vin.BoneWeights.z);
		vin.posL = mul(float4(vin.posL, 1.f), g_boneMatrices[vin.BoneIndices.w]) * BoneWeightW;
		vin.tangentL = mul(float4(vin.tangentL, 1.f), g_boneMatrices[vin.BoneIndices.w]) * BoneWeightW;
		vin.normalL = mul(float4(vin.tangentL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.w]) * BoneWeightW;
	}
#endif

	VertexOut vout;
	vout.posL = vin.posL;
	vout.normalL = vin.normalL;
	vout.tangentL = vin.tangentL;
	vout.uv = vin.uv;
	return vout;
}

float4 PS(DomainOut pin) : SV_Target
{
	pin.uv.x *= g_matUScale;
	pin.uv.y *= g_matVScale;

	MaterialColor materialColor;
	
	// Diffuse texture
	materialColor.diffuseAlbedo = g_diffuseColor;
	if (g_tex_on_diffuse == 1)
	{
		materialColor.diffuseAlbedo = g_texture_diffuse.Sample(g_sampler_anisotropicWrap, pin.uv);
	}

	// Specular texture
	materialColor.specular = g_specular;
	if (g_tex_on_specular == 1)
	{
		materialColor.specular = g_texture_specular.Sample(g_sampler_anisotropicWrap, pin.uv).xyz;
	}

	// Roughness texture
	materialColor.roughness = g_roughness;
	if (g_tex_on_roughness == 1)
	{
		materialColor.roughness = g_texture_roughness.Sample(g_sampler_linearWrap, pin.uv);
	}

	// Normal texture
	float3 normalMapW = pin.normalW;
	if (g_tex_on_normal == 1)
	{
		float3 normalMapSample = g_texture_normal.Sample(g_sampler_anisotropicWrap, pin.uv).xyz;
		normalMapW = NormalSampleToWorldSpace(normalMapSample, pin.normalW, pin.tangentW);
	}
        
	float3 eyePosW = GetCameraWorldPosition(g_viewInvMatrices[g_currentCameraIdx]);
	float shadowFactor = CalcShadowFactor(pin.shadowPosH1, pin.shadowPosH2, pin.shadowPosH3, pin.posH.w);
    return float4(ComputeLights(g_lights, g_lightCount, pin.posW, normalMapW, eyePosW, g_ambientLightColor, materialColor, shadowFactor), 1.f);
}

#endif // _FORWARD_TESS_LOD_