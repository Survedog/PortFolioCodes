#pragma once

#include "Collider.h"
#include "HeightMap.h"

class HeightMapCollider : public Collider
{
public:
	HeightMapCollider();
	HeightMapCollider(shared_ptr<HeightMap> heightMap);

public:
	inline shared_ptr<HeightMap> GetHeightMap() { return _heightMap; }
	inline shared_ptr<const HeightMap> GetHeightMap() const { return _heightMap; }

	inline void SetHeightMap(shared_ptr<HeightMap> newHeightMap)
	{ 
		if (_heightMap != nullptr)
		{
			// TODO: Remove old height map from resource manager.
		}
		_heightMap = newHeightMap;
	}

	inline uint32 GetRowCount()
	{
		if (_heightMap == nullptr) { /*Print error log.*/ return 0; }
		return _heightMap->GetRowCount();
	}

	inline uint32 GetColumnCount()
	{
		if (_heightMap == nullptr) { /*Print error log.*/ return 0; }
		return _heightMap->GetColumnCount();
	}

	inline float GetRowScale()
	{
		if (_heightMap == nullptr) { /*Print error log.*/ return 0; }
		return _heightMap->GetRowScale();
	}

	inline float GetColumnScale()
	{
		if (_heightMap == nullptr) { /*Print error log.*/ return 0; }
		return _heightMap->GetColumnScale();
	}

	inline float GetHeightScale()
	{
		if (_heightMap == nullptr) { /*Print error log.*/ return 0; }
		return _heightMap->GetHeightScale();
	}

private:
	shared_ptr<HeightMap> _heightMap;
};

