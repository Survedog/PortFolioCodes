#ifndef _TERRAIN_
#define _TERRAIN_

#pragma enable_d3d11_debug_symbols

#include "lightingUtils.hlsl"
#include "shadowUtils.hlsl"
#include "params.hlsl"
#include "utils.hlsl"

struct VertexIn
{
	float3 posL : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexOut
{
	float3 posL : POSITION;
	float2 uv : TEXCOORD;
};

struct HSConstants
{
	float edgeTessFactor[3] : SV_TessFactor;
	float insideTessFactor : SV_InsideTessFactor;
};

struct HullOut
{
	float3 posL : POSITION;
	float3 normalL : NORMAL;
	float3 tangentL : TANGENT;
	float2 uv : TEXCOORD;
};

struct DomainOut
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
	vout.posL = vin.posL;
	vout.uv = vin.uv;
	return vout;
}

HSConstants ConstantHS_LOD(InputPatch<VertexOut, 3> patch, uint patchId : SV_PrimitiveID)
{
	HSConstants constants;

	// Calculate tessellation factor based on the distance to the camera.
	float3 centerPosL = (patch[0].posL + patch[1].posL + patch[2].posL) / 3.f;
	float3 centerPosW = mul(float4(centerPosL, 1.f), g_worldMatrix).xyz;
	float3 cameraPosW = GetCameraWorldPosition(g_viewInvMatrices[g_currentCameraIdx]);
	float dist = distance(centerPosW, cameraPosW);

	const float d0 = 500.f;
	const float d1 = 2000.f;
	const float maxTessFactor = 64.f;
	const float minTessFactor = 1.f;
	float tessFactor = (maxTessFactor - minTessFactor) * saturate((d1 - dist) / (d1 - d0)) + minTessFactor;

	constants.edgeTessFactor[0] = tessFactor;
	constants.edgeTessFactor[1] = tessFactor;
	constants.edgeTessFactor[2] = tessFactor;
	constants.insideTessFactor = tessFactor;

	return constants;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS_LOD")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 3> patch,
	uint pointId : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.posL = patch[pointId].posL;
	hout.uv = patch[pointId].uv;

	uint texWidth, texHeight;
	g_texture_misc.GetDimensions(texWidth, texHeight);

	float dx = 1.f / texWidth;
	float dy = 1.f / texHeight;

	float rowScale = 10.f;
	float colScale = 10.f;

	// normal & tangent
	float height = g_texture_misc.SampleLevel(g_sampler_linearClamp, hout.uv, 0).x;
	float upHeight = g_texture_misc.SampleLevel(g_sampler_linearClamp, float2(hout.uv.x, hout.uv.y - dy), 0).x;
	float downHeight = g_texture_misc.SampleLevel(g_sampler_linearClamp, float2(hout.uv.x, hout.uv.y + dy), 0).x;
	float rightHeight = g_texture_misc.SampleLevel(g_sampler_linearClamp, float2(hout.uv.x + dx, hout.uv.y), 0).x;
	float leftHeight = g_texture_misc.SampleLevel(g_sampler_linearClamp, float2(hout.uv.x - dx, hout.uv.y), 0).x;

	float3 binormal = normalize(float3(2 * rowScale, (downHeight - upHeight) * 32767.f * g_terrainHeightScale, 0.f));
	hout.tangentL = normalize(float3(0.f, (rightHeight - leftHeight) * 32767.f * g_terrainHeightScale, 2 * colScale));
	hout.normalL = normalize(cross(hout.tangentL, binormal));

	return hout;
}

[domain("tri")]
DomainOut DS(HSConstants hsConstants,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> patch)
{
	DomainOut dout;

	dout.uv = BarycentricInterpolate(patch[0].uv, patch[1].uv, patch[2].uv, uvw);	
	
	float3 posL = BarycentricInterpolate(patch[0].posL, patch[1].posL, patch[2].posL, uvw);
	float height = g_texture_misc.SampleLevel(g_sampler_linearClamp, dout.uv, 0).x;
	height = (fmod(height + 0.5f, 1.00001f) - 0.5f) * 65535.f / 2.f; // Convert UNORM range(0.0~1.0) to SINT range(-32768~32767)
	posL.y = height * g_terrainHeightScale;

	dout.posW = mul(float4(posL, 1.f), g_worldMatrix).xyz;
	dout.posH = mul(float4(dout.posW, 1.f), g_viewProjMatrices[g_currentCameraIdx]);
	dout.posNDC = dout.posH; // Warning: This position is actually in homogeneous clip space until the w-division happens in the rasterizer stage.
	
	dout.shadowPosH1 = mul(float4(dout.posW, 1.f), g_shadowMatrices[0]);
	dout.shadowPosH2 = mul(float4(dout.posW, 1.f), g_shadowMatrices[1]);
	dout.shadowPosH3 = mul(float4(dout.posW, 1.f), g_shadowMatrices[2]);

	dout.normalW = normalize(BarycentricInterpolate(patch[0].normalL, patch[1].normalL, patch[2].normalL, uvw));
	dout.normalW = normalize(mul(dout.normalW, (float3x3)g_worldInvTransposeMatrix));
	
	dout.tangentW = normalize(BarycentricInterpolate(patch[0].tangentL, patch[1].tangentL, patch[2].tangentL, uvw));
	dout.tangentW = normalize(mul(dout.tangentW, (float3x3)g_worldMatrix));

	return dout;
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

#endif // _TERRAIN_