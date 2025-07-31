#include "pch.h"
#include "Collider.h"

Collider::Collider(CollisionShape shape) :	
	_shape(shape),
	_offset(Vec3::Zero)
{
}
