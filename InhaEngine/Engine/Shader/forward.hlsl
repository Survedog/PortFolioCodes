#ifndef _FORWARD_
#define _FORWARD_

#pragma enable_d3d11_debug_symbols

#include "lightingUtils.hlsl"
#include "shadowUtils.hlsl"
#include "params.hlsl"
#include "utils.hlsl"

struct VertexIn
{
    float3 posL : POSITION;
    float2 uv : TEXCOORD;
    float3 normalL : NORMAL;
    float3 tangentL : TANGENT;

#ifdef SKINNED
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
#endif
};

struct VertexOut
{
    float4 posNDC : SV_Position;
    float3 posW : POSITION0;
	float4 posH : POSITION1;
    float4 shadowPosH1 : POSITION2;
	float4 shadowPosH2 : POSITION3;
	float4 shadowPosH3 : POSITION4;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

#ifdef SKINNED
	//vin.posL = mul(float4(vin.posL, 1.f), g_boneMatrices[vin.BoneIndices.x]);
	//vin.tangentL = mul(float4(vin.tangentL, 1.f), g_boneMatrices[vin.BoneIndices.x]);
	//vin.normalL = mul(float4(vin.tangentL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.x]);

	float3 originalPosL = vin.posL;
	float3 originalTangentL = vin.tangentL;
	float3 originalNormalL = vin.normalL;

	vin.posL = float3(0.f, 0.f, 0.f);
	vin.tangentL = float3(0.f, 0.f, 0.f);
	vin.normalL = float3(0.f, 0.f, 0.f);

	if (vin.BoneIndices.x < MAX_SKELETON_BONES)
	{
		vin.posL += mul(float4(originalPosL, 1.f), g_boneMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
		vin.tangentL += mul(float4(originalTangentL, 1.f), g_boneMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
		vin.normalL += mul(float4(originalNormalL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.x]) * vin.BoneWeights.x;
	}

	if (vin.BoneIndices.y < MAX_SKELETON_BONES)
	{
		vin.posL += mul(float4(originalPosL, 1.f), g_boneMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
		vin.tangentL += mul(float4(originalTangentL, 1.f), g_boneMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
		vin.normalL += mul(float4(originalNormalL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.y]) * vin.BoneWeights.y;
	}

	if (vin.BoneIndices.z < MAX_SKELETON_BONES)
	{
		vin.posL += mul(float4(originalPosL, 1.f), g_boneMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
		vin.tangentL += mul(float4(originalTangentL, 1.f), g_boneMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
		vin.normalL += mul(float4(originalNormalL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.z]) * vin.BoneWeights.z;
	}

	if (vin.BoneIndices.w < MAX_SKELETON_BONES)
	{
		float BoneWeightW = 1.f - (vin.BoneWeights.x + vin.BoneWeights.y + vin.BoneWeights.z);
		vin.posL += mul(float4(originalPosL, 1.f), g_boneMatrices[vin.BoneIndices.w]) * BoneWeightW;
		vin.tangentL += mul(float4(originalTangentL, 1.f), g_boneMatrices[vin.BoneIndices.w]) * BoneWeightW;
		vin.normalL += mul(float4(originalNormalL, 1.f), g_boneInvTransposeMatrices[vin.BoneIndices.w]) * BoneWeightW;
	}
#endif

    vout.posW = mul(float4(vin.posL, 1.f), g_worldMatrix).xyz;
    vout.posH = mul(float4(vout.posW, 1.f), g_viewProjMatrices[g_currentCameraIdx]);
	vout.posNDC = vout.posH; // Warning: This position is actually in homogeneous clip space until the w-division happens in the rasterizer stage.
    vout.shadowPosH1 = mul(float4(vout.posW, 1.f), g_shadowMatrices[0]);
	vout.shadowPosH2 = mul(float4(vout.posW, 1.f), g_shadowMatrices[1]);
	vout.shadowPosH3 = mul(float4(vout.posW, 1.f), g_shadowMatrices[2]);
    vout.normalW = normalize(mul(vin.normalL, (float3x3)g_worldInvTransposeMatrix));
    vout.tangentW = normalize(mul(vin.tangentL, (float3x3)g_worldMatrix)); // TODO: Check if it's correct.   
    vout.uv = vin.uv;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
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
		materialColor.roughness = g_texture_roughness.Sample(g_sampler_linearWrap, pin.uv).x;
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

#endif // _FORWARD_