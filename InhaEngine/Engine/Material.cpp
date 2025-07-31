#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "TableDescriptorHeapManager.h"
#include "CommandQueueManager.h"
#include "Texture.h"

Material::Material() :
	Object(OBJECT_TYPE::MATERIAL, L"Material_default"),
	_shader(nullptr),
	_textures(TEXTURE_TYPE_COUNT, nullptr),
	_textureCubes(MAX_TEXTURE_CUBES, nullptr)
{
}

void Material::PushData()
{
	assert(_textures.size() + _textureCubes.size() <= MATERIAL_TABLE_SRV_COUNT);

	ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::MATERIAL);

	for (uint8 i = 0; i < _textures.size(); ++i)
	{
		DescTableRegister reg = { static_cast<uint8>(MATERIAL_TABLE_REGISTER::t0) + i };
		if (_textures[i] != nullptr)
			ENGINE->GetGraphicsDescHeapManager()->SetDesc(_textures[i]->GetSRVHeapHandle(), DESC_TABLE_TYPE::MATERIAL, reg);
	}

	for (uint8 i = 0; i < _textureCubes.size(); ++i)
	{
		DescTableRegister reg = { static_cast<uint8>(MATERIAL_TABLE_REGISTER::t13) + i };
		if (_textureCubes[i] != nullptr)
			ENGINE->GetGraphicsDescHeapManager()->SetDesc(_textureCubes[i]->GetSRVHeapHandle(), DESC_TABLE_TYPE::MATERIAL, reg);
	}

	ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::PER_MATERIAL)->PushGraphicsConstants(&_params, sizeof(MaterialConstants));	
	ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::MATERIAL), MATERIAL_TABLE_DESC_COUNT);
}

void Material::ApplyShader(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	_shader->ApplyShader(commandList);
}
