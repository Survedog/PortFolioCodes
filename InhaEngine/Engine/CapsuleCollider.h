#pragma once

#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(float radius, float halfHeight)
		: Collider(CollisionShape::Capsule), _radius(radius), _halfHeight(halfHeight)
	{
	}

	inline float GetRadius() const { return _radius; }
	inline void SetRadius(float newRadius) { _radius = newRadius; }

	inline float GetHalfHeight() const { return _halfHeight; }
	inline void SetHalfHeight(float newHalfHeight) { _halfHeight = newHalfHeight; }

private:
	float _radius = 1.f;
	float _halfHeight = 1.f;
};

