#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "DeviceManager.h"
#include "CommandQueueManager.h"
#include "TableDescriptorHeapManager.h"
#include "TransformComponent.h"
#include "Material.h"
#include "ResourceManager.h"

Mesh::Mesh() :
	Object(OBJECT_TYPE::STATIC_MESH)
{
}

Mesh::Mesh(Mesh&& mesh) noexcept :
	Object(OBJECT_TYPE::STATIC_MESH),
	_vertexBuffer(move(mesh._vertexBuffer)),
	_indexBuffer(move(mesh._indexBuffer)),
	_subMeshInfoMap(move(mesh._subMeshInfoMap)),
	_materials(move(mesh._materials))
{
	_vertexBufferView = mesh._vertexBufferView;
	_vertexTotalCount = mesh._vertexTotalCount;
	_indexBufferView = mesh._indexBufferView;
	_indexTotalCount = mesh._indexTotalCount;

	// Clear move-from object's member variables to prevent their deletion.
	mesh._vertexBuffer = nullptr;
	mesh._indexBuffer = nullptr;
	mesh._subMeshInfoMap.clear();
	mesh._materials.clear();
}

void Mesh::Load(const wstring& path)
{
	// TODO: Create mesh using fbx importer
}

void Mesh::Create(const vector<Vertex>& vertices, const vector<uint32>& indices, shared_ptr<Material> material, bool addSubMeshManually)
{
	vector<shared_ptr<Material>> materials;
	if (material != nullptr) materials.push_back(move(material));
	Create(vertices, indices, materials, addSubMeshManually);
}

void Mesh::Create(const vector<Vertex>& vertices, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually)
{
	const void* pVerticesData = reinterpret_cast<const void*>(vertices.data());
	CreateVertexBuffer(pVerticesData, static_cast<uint32>(vertices.size()), sizeof(Vertex));
	CreateIndexBuffer(indices);
	_materials = materials;

	if (!addSubMeshManually)
	{
		SubMeshInfo subMeshInfo(GetIndexTotalCount(), 0, 0, 0);
		AddSubMeshInfo(L"Main", subMeshInfo);
	}
}

void Mesh::Create(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, shared_ptr<Material> material, bool addSubMeshManually)
{
	vector<shared_ptr<Material>> materials;
	if (material != nullptr) materials.push_back(move(material));
	Create(pVerticesData, vertexCount, vertexByteSize, indices, materials, addSubMeshManually);
}

void Mesh::Create(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually)
{
	CreateVertexBuffer(pVerticesData, vertexCount, vertexByteSize);
	CreateIndexBuffer(indices);
	_materials = materials;

	if (!addSubMeshManually)
	{
		SubMeshInfo subMeshInfo(GetIndexTotalCount(), 0, 0, 0);
		AddSubMeshInfo(L"Main", subMeshInfo);
	}
}

void Mesh::CreateVertexBuffer(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize)
{
	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	assert(device != nullptr);

	_vertexTotalCount = vertexCount;
	_vertexByteSize = vertexByteSize;
	const uint32 bufferSize = vertexByteSize * vertexCount;

	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC heapDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&heapDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	// Copy vertex info to vertex buffer
	void* vertexBufferCPU = nullptr;
	CD3DX12_RANGE readRange(0, 0); // Write-only
	_vertexBuffer->Map(0, &readRange, &vertexBufferCPU);
	memcpy(vertexBufferCPU, pVerticesData, bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.SizeInBytes = bufferSize;
	_vertexBufferView.StrideInBytes = vertexByteSize;
}

void Mesh::CreateIndexBuffer(const vector<uint32>& indices)
{
	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	assert(device != nullptr);

	_indexTotalCount = static_cast<uint32>(indices.size());
	const uint32 bufferSize = sizeof(uint32) * _indexTotalCount;

	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC heapDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&heapDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer));

	// Copy index info to index buffer
	void* indexBufferCPU = nullptr;
	CD3DX12_RANGE readRange(0, 0); // Write-only
	_indexBuffer->Map(0, &readRange, &indexBufferCPU);
	memcpy(indexBufferCPU, &indices[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr);

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;
}

void Mesh::Render(shared_ptr<TransformComponent> transformComponent) const
{
	OnPreRender();

	ComPtr<ID3D12GraphicsCommandList> commandList = ENGINE->GetCommandQueueManager()->GetGraphicsCommandList();
	commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	commandList->IASetIndexBuffer(&_indexBufferView);

	if (transformComponent != nullptr)
	{
		transformComponent->PushData();
	}	

	// TODO: Draw every gameobjects that shares material at once.
	for (const pair<wstring, SubMeshInfo>& pair : _subMeshInfoMap)
	{
		const SubMeshInfo& subMeshInfo = pair.second;
		shared_ptr<Material> material = _materials[subMeshInfo.materialIndex];
		
		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		material->ApplyShader(commandList);
		material->PushData();
		
		commandList->DrawIndexedInstanced(subMeshInfo.indexCount, 1, subMeshInfo.startIndexLocation, subMeshInfo.baseVertexLocation, 0);
	}
}

void Mesh::RenderShadowMap(shared_ptr<class TransformComponent> transformComponent) const
{
	ComPtr<ID3D12GraphicsCommandList> commandList = ENGINE->GetCommandQueueManager()->GetGraphicsCommandList();
	commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	commandList->IASetIndexBuffer(&_indexBufferView);

	transformComponent->PushData();

	shared_ptr<Material> shadowMaterial = ResourceManager::GetInstance()->FindMaterial(L"Material_shadowMap");
	shadowMaterial->PushData();
	shadowMaterial->ApplyShader(commandList);

	for (const pair<wstring, SubMeshInfo>& pair : _subMeshInfoMap)
	{
		const SubMeshInfo& subMeshInfo = pair.second;
		commandList->DrawIndexedInstanced(subMeshInfo.indexCount, 1, subMeshInfo.startIndexLocation, subMeshInfo.baseVertexLocation, 0);
	}
}
