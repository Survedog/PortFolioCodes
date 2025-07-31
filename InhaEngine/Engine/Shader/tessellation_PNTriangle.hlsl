#ifndef _TESSELLATION_PNTRIANGLE_
#define _TESSELLATION_PNTRIANGLE_

#pragma enable_d3d11_debug_symbols

#include "params.hlsl"
#include "utils.hlsl"

struct HSConstants
{
	float edgeTessFactor[3] : SV_TessFactor;
	float insideTessFactor : SV_InsideTessFactor;

	float3 b120 : POSITION3;
	float3 b210 : POSITION4;
	float3 b012 : POSITION5;
	float3 b021 : POSITION6;
	float3 b102 : POSITION7;
	float3 b201 : POSITION8;
	float3 b111 : POSITION9;

	float3 n110 : NORMAL3;
	float3 n011 : NORMAL4;
	float3 n101 : NORMAL5;
};

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
	float3 posL : POSITION;
	float2 uv : TEXCOORD;
	float3 normalL : NORMAL;
	float3 tangentL : TANGENT;
};

struct HullOut
{
	float3 posL : POSITION;
	float2 uv : TEXCOORD;
	float3 normalL : NORMAL;
	float3 tangentL : TANGENT;
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

float3 CalcEdgeControlPoint(float3 pi, float3 pj, float3 ni)
{
	return (2.f * pi + pj - dot(pj - pi, ni) * ni) / 3.f;
}

float3 CalcEdgeNormal(float3 pi, float3 pj, float3 ni, float3 nj)
{
	return normalize(ni + nj - 2.f * (dot(ni + nj, pi - pj) / dot(pi - pj, pi - pj)) * (pi - pj));
}

HSConstants ConstantHS_LOD(InputPatch<VertexOut, 3> patch,
	uint patchId : SV_PrimitiveID)
{
	HSConstants constants;
	
	// Calculate tessellation factor based on the distance to the camera.
	float3 centerPosL = (patch[0].posL + patch[1].posL + patch[2].posL) / 3.f;
	float3 centerPosW = mul(float4(centerPosL, 1.f), g_worldMatrix).xyz;
	float3 cameraPosW = GetCameraWorldPosition(g_viewInvMatrices[g_currentCameraIdx]);
	float dist = distance(centerPosW, cameraPosW);

	const float d0 = 50.f;
	const float d1 = 300.f;
	const float maxTessFactor = 6.f;
	const float minTessFactor = 1.f;
	float tessFactor = (maxTessFactor - minTessFactor) * saturate((d1 - dist) / (d1 - d0)) + minTessFactor;
	//float tessFactor = 3.f;

	constants.edgeTessFactor[0] = tessFactor;
	constants.edgeTessFactor[1] = tessFactor;
	constants.edgeTessFactor[2] = tessFactor;
	constants.insideTessFactor = tessFactor;

	// Calculate PN-triangle control points.
	float3 p1 = patch[0].posL;
	float3 p2 = patch[1].posL;
	float3 p3 = patch[2].posL;
	
	float3 n1 = patch[0].normalL;
	float3 n2 = patch[1].normalL;
	float3 n3 = patch[2].normalL;

	constants.b120 = CalcEdgeControlPoint(p2, p1, n2);
	constants.b210 = CalcEdgeControlPoint(p1, p2, n1);
	constants.b012 = CalcEdgeControlPoint(p3, p2, n3);
	constants.b021 = CalcEdgeControlPoint(p2, p3, n2);
	constants.b102 = CalcEdgeControlPoint(p3, p1, n3);
	constants.b201 = CalcEdgeControlPoint(p1, p3, n1);

	float3 e = (constants.b120 + constants.b210 + constants.b012 + constants.b021 + constants.b102 + constants.b201) / 6.f;
	float3 v = (p1 + p2 + p3) / 3.f;
	constants.b111 = e + (e - v) * 0.5f;

	// Calculate PN-triangle normals.
	constants.n110 = CalcEdgeNormal(p1, p2, n1, n2);
	constants.n011 = CalcEdgeNormal(p2, p3, n2, n3);
	constants.n101 = CalcEdgeNormal(p1, p3, n1, n3);

	return constants;
}

[domain("tri")]
[partitioning("fractional_odd")]
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
	hout.normalL = patch[pointId].normalL;
	hout.tangentL = patch[pointId].tangentL;
	hout.uv = patch[pointId].uv;

	return hout;
}

[domain("tri")]
DomainOut DS(HSConstants hsConstants,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> patch)
{
	DomainOut dout;

	// Calculate vertex position and normal on the bezier triangle.
	float u = uvw.x;		float v = uvw.y;		float w = uvw.z;
	float uu = u * u;		float vv = v * v;		float ww = w * w;

	float3 b300 = patch[0].posL;
	float3 b030 = patch[1].posL;
	float3 b003 = patch[2].posL;

	float3 n200 = patch[0].normalL;
	float3 n020 = patch[1].normalL;
	float3 n002 = patch[2].normalL;

	float3 posL = b300 * u * uu + b030 * v * vv + b003 * w * ww
		+ hsConstants.b120 * u * vv * 3.f
		+ hsConstants.b210 * v * uu * 3.f
		+ hsConstants.b012 * v * ww * 3.f
		+ hsConstants.b021 * w * vv * 3.f
		+ hsConstants.b102 * u * ww * 3.f
		+ hsConstants.b201 * w * uu * 3.f
		+ hsConstants.b111 * u * v * w * 6.f;

	float3 normalL = n200 * uu + n020 * vv + n002 * ww
		+ 2.f * u * v * hsConstants.n110
		+ 2.f * v * w * hsConstants.n011
		+ 2.f * u * w * hsConstants.n101;

	dout.posW = mul(float4(posL, 1.f), g_worldMatrix).xyz;
	dout.posH = mul(float4(dout.posW, 1.f), g_viewProjMatrices[g_currentCameraIdx]);
	dout.posNDC = dout.posH; // Warning: This position is actually in homogeneous clip space until the w-division happens in the rasterizer stage.
	
	dout.shadowPosH1 = mul(float4(dout.posW, 1.f), g_shadowMatrices[0]);
	dout.shadowPosH2 = mul(float4(dout.posW, 1.f), g_shadowMatrices[1]);
	dout.shadowPosH3 = mul(float4(dout.posW, 1.f), g_shadowMatrices[2]);

	dout.normalW = normalize(mul(normalL, (float3x3)g_worldInvTransposeMatrix));

	dout.tangentW = BarycentricInterpolate(patch[0].tangentL, patch[1].tangentL, patch[2].tangentL, uvw);
	dout.tangentW = normalize(mul(dout.tangentW, (float3x3)g_worldMatrix));
	dout.uv = BarycentricInterpolate(patch[0].uv, patch[1].uv, patch[2].uv, uvw);

	return dout;
}

#endif // _TESSELLATION_PNTRIANGLE_