#include "pch.h"
#include "FrameResource.h"
#include "Material.h"

FrameResource::FrameResource(ID3D12Device* device, uint32 passCount)
{
	THROW_MSG_DX_IF_FAILED(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_cmdAlloc.GetAddressOf()))
		, L"Failed to create command allocator.");

	CreateConstantBuffer(CONSTANT_BUFFER_TYPE::PER_MATERIAL, sizeof(MaterialConstants), MAX_MATERIALS);
	CreateConstantBuffer(CONSTANT_BUFFER_TYPE::PER_PASS, sizeof(PerPassConstants), passCount);
	CreateConstantBuffer(CONSTANT_BUFFER_TYPE::GLOBAL, sizeof(GlobalConstants), 1);
	CreateConstantBuffer(CONSTANT_BUFFER_TYPE::PER_GAMEOBJECT, sizeof(PerGameObjectConstants), MAX_GAMEOBJECTS);
	CreateConstantBuffer(CONSTANT_BUFFER_TYPE::PER_ANIMATION, sizeof(PerAnimationConstants), MAX_ANIMATION_SEQUENCES);
}

void FrameResource::CreateConstantBuffer(CONSTANT_BUFFER_TYPE type, uint32 bufferSize, uint32 count)
{
	shared_ptr<ConstantBuffer> constBuffer = make_shared<ConstantBuffer>();
	constBuffer->Init(type, bufferSize, count);
	_constantBuffers[static_cast<uint8>(type)] = constBuffer;
}
