#pragma once

#include "Object.h"

struct SubMeshInfo
{
	SubMeshInfo() : indexCount(0), baseVertexLocation(0), startIndexLocation(0), materialIndex(0)
	{
	}

	SubMeshInfo(uint32 indexCount,
				int32 baseVertexLocation,
				uint32 startIndexLocation,
				uint32 materialIndex) :
		indexCount(indexCount),
		baseVertexLocation(baseVertexLocation),
		startIndexLocation(startIndexLocation),
		materialIndex(materialIndex)
	{
	}

	SubMeshInfo(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
				const D3D12_INDEX_BUFFER_VIEW& indexBufferView,
				uint32 indexCount,
				int32 baseVertexLocation,
				uint32 startIndexLocation,
				uint32 materialIndex) :
		vertexBufferView(vertexBufferView),
		indexBufferView(indexBufferView),
		indexCount(indexCount),
		baseVertexLocation(baseVertexLocation),
		startIndexLocation(startIndexLocation),
		materialIndex(materialIndex)
	{
	}

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

	int32 baseVertexLocation = 0;
	uint32 startIndexLocation = 0;	
	uint32 indexCount = 0;

	uint32 materialIndex = 0;
};

class Material;

// Represents a static mesh.
class Mesh : public Object
{
public:
	Mesh();
	Mesh(Mesh&& mesh) noexcept;
	virtual ~Mesh() = default;

public:
	virtual void Load(const wstring& path) override;

	// For general meshes
	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	void Create(const vector<Vertex>& vertices, const vector<uint32>& indices, shared_ptr<Material> material = nullptr, bool addSubMeshManually = false);

	// For general meshes
	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	void Create(const vector<Vertex>& vertices, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually = false);

	// For special meshes
	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	void Create(const void* const pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, shared_ptr<Material> material = nullptr, bool addSubMeshManually = false);

	// For special meshes
	// NOTE: If you set addSubMeshManually false, then a submesh that represents the whole mesh will be added automatically.
	// Otherwise you need to add submesh information manually using Mesh::AddSubMeshInfo.
	void Create(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize, const vector<uint32>& indices, const vector<shared_ptr<Material>>& materials, bool addSubMeshManually = false);

public:
	void Render(shared_ptr<class TransformComponent> transformComponent) const;
	void RenderShadowMap(shared_ptr<class TransformComponent> transformComponent) const;

protected:
	virtual void OnPreRender() const {};

private:
	void CreateVertexBuffer(const void* pVerticesData, uint32 vertexCount, uint32 vertexByteSize);
	void CreateIndexBuffer(const vector<uint32>& indices);

public:
	inline ComPtr<ID3D12Resource> GetVertexBuffer() { return _vertexBuffer; }
	inline ComPtr<ID3D12Resource> GetIndexBuffer() { return _indexBuffer; }
	
	inline D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return _vertexBufferView; }
	inline const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return _vertexBufferView; }

	inline D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _indexBufferView; }
	inline const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return _indexBufferView; }

	inline uint32 GetVertexTotalCount() { return _vertexTotalCount; }
	inline uint32 GetIndexTotalCount() { return _indexTotalCount; }

	inline uint32 GetVertexByteSize() { return _vertexByteSize; }

	// Find submesh information by key. Returns nullopt if not found.
	inline optional<SubMeshInfo> FindSubMeshInfo(const wstring& key)
	{
		auto findIter = _subMeshInfoMap.find(key);
		if (findIter == _subMeshInfoMap.end()) return nullopt;
		return findIter->second;
	}

	inline void GetSubMeshesInfo(vector<SubMeshInfo>& outInfo)
	{
		for (const pair<wstring, SubMeshInfo>& pair : _subMeshInfoMap)
			outInfo.push_back(pair.second);
	}

	// WARNING: Do not call this function before Mesh::Create.
	inline void AddSubMeshInfo(const wstring& key, SubMeshInfo& subMeshInfo)
	{
		subMeshInfo.vertexBufferView = _vertexBufferView;
		subMeshInfo.indexBufferView = _indexBufferView;
		_subMeshInfoMap[key] = subMeshInfo;
	}

private:
	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
	uint32 _vertexTotalCount = 0;
	uint32 _vertexByteSize = 0;

	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};
	uint32 _indexTotalCount = 0;

	map<wstring, SubMeshInfo> _subMeshInfoMap;

public:
	inline shared_ptr<Material> GetMaterial(int idx) { return idx < _materials.size() ? _materials[idx] : nullptr; }
	inline shared_ptr<const Material> GetMaterial(int idx) const { return idx < _materials.size() ? _materials[idx] : nullptr; }
	inline vector<shared_ptr<Material>>& GetMaterials() { return _materials; }
	inline const vector<shared_ptr<Material>>& GetMaterials() const { return _materials; }
	inline void SetMaterials(vector<shared_ptr<Material>>& newMaterials) { _materials = newMaterials; }

	inline void AddMaterial(shared_ptr<Material> newMaterial) { _materials.push_back(move(newMaterial)); }
	inline void ChangeMaterial(uint32 materialIdx, shared_ptr<Material> newMaterial) { assert(materialIdx < _materials.size()); _materials[materialIdx] = newMaterial; }
	inline void RemoveMaterial(uint32 materialIdx) { _materials.erase(_materials.begin() + materialIdx); }

private:
	vector<shared_ptr<Material>> _materials;
};

