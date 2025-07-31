#ifndef _SKYBOX_
#define _SKYBOX_

#pragma enable_d3d11_debug_symbols

#include "params.hlsl"
#include "utils.hlsl"

struct VertexIn
{
	float3 posL : POSITION;
};

struct VertexOut
{
	float4 posNDC : SV_Position;
	float3 posL : POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

	// Ignore translation to place skybox's center at the camera position.
	// Put w value to z to draw skybox at the farthest depth (w(=z)/w = 1.f)
	vout.posNDC = mul(float4(vin.posL, 0.f), g_viewProjMatrices[g_currentCameraIdx]).xyww; // Warning: This position is actually in homogeneous clip space until the w-division happens in the rasterizer stage.
	vout.posL = vin.posL;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{       
    return g_textureCube_skybox.Sample(g_sampler_pointWrap, normalize(pin.posL));
}

#endif // _SKYBOX_