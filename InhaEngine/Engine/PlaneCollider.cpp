#include "pch.h"
#include "PlaneCollider.h"

PlaneCollider::PlaneCollider(Vec3 normal, Vec3 point) :
	Collider(CollisionShape::Plane), _normal(normal), _point(point)
{
}

PlaneCollider::PlaneCollider(float normalX, float normalY, float normalZ, Vec3 point) :
	PlaneCollider(Vec3(normalX, normalY, normalZ), point)
{
}

PlaneCollider::PlaneCollider(Vec3 normal, float pointX, float pointY, float pointZ) :
	PlaneCollider(normal, Vec3(pointX, pointY, pointZ))
{
}

PlaneCollider::PlaneCollider(float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ) :
	PlaneCollider(Vec3(normalX, normalY, normalZ), Vec3(pointX, pointY, pointZ))
{
}
