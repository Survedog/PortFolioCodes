#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius = 1.f) :
		Collider(CollisionShape::Sphere), _radius(radius)
	{
	}

	inline float GetRadius() const { return _radius; }
	inline void SetRadius(float newRadius) { _radius = newRadius; }

private:
	float _radius = 1.f;
};

