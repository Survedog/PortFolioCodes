#include "pch.h"
#include "HeightMapCollider.h"
#include "HeightMap.h"
#include "ResourceManager.h"

HeightMapCollider::HeightMapCollider() :
	Collider(CollisionShape::HeightField), _heightMap(nullptr)
{
}

HeightMapCollider::HeightMapCollider(shared_ptr<HeightMap> heightMap) :
	Collider(CollisionShape::HeightField), _heightMap(heightMap)
{
}
