#include "pch.h"
#include "BoxCollider.h"

BoxCollider::BoxCollider(Vec3 halfExtents) :
	Collider(CollisionShape::Box), _halfExtents(halfExtents)
{	
}

BoxCollider::BoxCollider(float squareHalfExtent) :
	BoxCollider(Vec3(squareHalfExtent))
{
}

BoxCollider::BoxCollider(float xHalfExtent, float yHalfExtent, float zHalfExtent) :
	BoxCollider(Vec3(xHalfExtent, yHalfExtent, zHalfExtent))
{
}
