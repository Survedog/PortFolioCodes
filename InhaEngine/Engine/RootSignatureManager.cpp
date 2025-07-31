#include "pch.h"
#include "RootSignatureManager.h"
#include "Engine.h"
#include "DeviceManager.h"

void RootSignatureManager::Init()
{
	CreateGraphicsRootSignature();
}

// TODO: Make it customizable.
void RootSignatureManager::CreateGraphicsRootSignature()
{
	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	assert(device);

	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_PARAMETER rootParams[6] = {};

	// Material
	CD3DX12_DESCRIPTOR_RANGE materialDescTable[2] = {};
	materialDescTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, static_cast<uint32>(CBV_REGISTER::b0)); // b0
	materialDescTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, static_cast<uint32>(MATERIAL_TABLE_SRV_COUNT), static_cast<uint32>(SRV_REGISTER::t0)); // t0~t13
	rootParams[0].InitAsDescriptorTable(_countof(materialDescTable), materialDescTable);

	// Pass (camera, ...)
	CD3DX12_DESCRIPTOR_RANGE passDescTable(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); // b1
	rootParams[1].InitAsDescriptorTable(1, &passDescTable);

	// Global (lights, time, ...)
	rootParams[2].InitAsConstantBufferView(static_cast<uint32>(CBV_REGISTER::b2));

	// GlobalTexture (ShadowMap, Reflection CubeMap, ...)
	CD3DX12_DESCRIPTOR_RANGE globalTextureDescTable[1] = {};
	globalTextureDescTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, static_cast<uint32>(SRV_REGISTER::t17)); // t17~t20
	rootParams[3].InitAsDescriptorTable(1, globalTextureDescTable, D3D12_SHADER_VISIBILITY_PIXEL);

	// Transform (TODO: Use PER_INSTANCE vertex data instead)
	CD3DX12_DESCRIPTOR_RANGE transformDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3); // b3
	rootParams[4].InitAsDescriptorTable(1, &transformDescRange);

	// Animation
	CD3DX12_DESCRIPTOR_RANGE animationDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 4); // b4
	rootParams[5].InitAsDescriptorTable(1, &animationDescRange);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(rootParams), rootParams, staticSamplers.size(), staticSamplers.data());
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);	
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));
}

vector<D3D12_STATIC_SAMPLER_DESC> RootSignatureManager::GetStaticSamplers() const
{
	const uint32 maxAnisotropy = 8U;

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	const CD3DX12_STATIC_SAMPLER_DESC anisotrophicWrap(
		4,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		maxAnisotropy);

	const CD3DX12_STATIC_SAMPLER_DESC anisotrophicClamp(
		5,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.0f,
		maxAnisotropy);

	const CD3DX12_STATIC_SAMPLER_DESC comparisonPointBorder(
		6,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		0.0f, 16U,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

	return { pointWrap, pointClamp, linearWrap, linearClamp, anisotrophicWrap, anisotrophicClamp, comparisonPointBorder };
}
