#include "pch.h"
#include "TerrainComponent.h"
#include "ResourceManager.h"
#include "HeightMapCollider.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceManager.h"

TerrainComponent::TerrainComponent() :
	Component(COMPONENT_TYPE::TERRAIN, UPDATE_GROUP::DuringPhysics)
{
}

void TerrainComponent::InitWithMaterial(shared_ptr<Material> terrainMaterial)
{
	if (terrainMaterial == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	shared_ptr<Texture> heightMapTexture = terrainMaterial->GetMiscTexture();
	if (heightMapTexture == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	shared_ptr<HeightMap> heightMap = make_shared<HeightMap>(heightMapTexture);
	if (GetHeightMapCollider() == nullptr) _heightMapCollider = make_shared<HeightMapCollider>();
	_heightMapCollider->SetHeightMap(heightMap);

	uint32 rowCount = _heightMapCollider->GetRowCount() / 8; // Divide for LOD tessellation
	uint32 colCount = _heightMapCollider->GetColumnCount() / 8; // Divide for LOD tessellation
	float rowScale = _heightMapCollider->GetRowScale() * 8.f;
	float colScale = _heightMapCollider->GetColumnScale() * 8.f;

	_heightMapMesh = GeometryGenerator::CreateHeightMapGridMesh(rowCount, colCount, rowScale, colScale);
	_heightMapMesh->AddMaterial(terrainMaterial);
	ResourceManager::GetInstance()->Add(L"Mesh_HeightMapGrid" + _heightMapCollider->GetHeightMap()->GetID(), _heightMapMesh);
}

shared_ptr<Material> TerrainComponent::GetHeightMapMaterial()
{
	return _heightMapMesh != nullptr ? _heightMapMesh->GetMaterial(0) : nullptr;
}

void TerrainComponent::OnOwnerGameObjectSet(GameObject* newOwnerGameObject)
{
	ENGINE->GetPhysicsManager()->RegisterGameObject(newOwnerGameObject->GetSharedPtr());
}
