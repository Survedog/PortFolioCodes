#ifndef _FORWARD_
#define _FORWARD_

#pragma enable_d3d11_debug_symbols

#include "params.hlsl"
#include "utils.hlsl"

struct VertexIn
{
    float3 posL : POSITION;
    float2 uv : TEXCOORD;

#ifdef SKINNED
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
#endif
};

struct VertexOut
{
    float3 posW : POSITION;
    float4 posNDC : SV_Position;
    float2 uv : TEXCOORD;
};

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
    vout.posW = mul(float4(vin.posL, 1.0f), g_worldMatrix);
	vout.posNDC = mul(float4(vout.posW, 1.0f), g_shadowCam_viewProjMatrices[g_currentCameraIdx]); // Warning: This position is actually in homogeneous clip space until the w-division happens in the rasterizer stage.
    vout.uv = vin.uv;
    return vout;
}

void PS(VertexOut pin)
{
    float4 diffuseAlbedo = g_texture_diffuse.Sample(g_sampler_pointWrap, pin.uv);
    
#ifdef ALPHA_TEST
    clip(diffuseAlbedo.a - 0.1f);
#endif
}

#endif // _FORWARD_