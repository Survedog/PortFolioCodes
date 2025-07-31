#pragma once

#include "Component.h"
#include "HeightMapCollider.h"

class Mesh;
class Material;
class HeightMap;

class TerrainComponent : public Component
{
public:
	TerrainComponent();

	virtual ~TerrainComponent() = default;

public:
	void InitWithMaterial(shared_ptr<Material> terrainMaterial);

	inline void RemoveHeightMap() { _heightMapCollider->SetHeightMap(nullptr); }

	inline shared_ptr<HeightMapCollider> GetHeightMapCollider() { return _heightMapCollider; }
	inline shared_ptr<const HeightMapCollider> GetHeightMapCollider() const { return _heightMapCollider; }
	inline shared_ptr<HeightMap> GetHeightMap() { return GetHeightMapCollider() != nullptr ? GetHeightMapCollider()->GetHeightMap() : nullptr; }

	inline shared_ptr<Mesh> GetHeightMapMesh() { return _heightMapMesh; }
	shared_ptr<Material> GetHeightMapMaterial();

protected:
	virtual void OnOwnerGameObjectSet(GameObject* newOwnerGameObject) override;

private:
	shared_ptr<HeightMapCollider> _heightMapCollider = nullptr;
	shared_ptr<Mesh> _heightMapMesh = nullptr;
};

