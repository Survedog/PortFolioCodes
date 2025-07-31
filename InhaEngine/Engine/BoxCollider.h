#pragma once

#include "Collider.h"

class BoxCollider : public Collider
{
public:
	BoxCollider(Vec3 halfExtents);
	BoxCollider(float squareHalfExtent);
	BoxCollider(float xHalfExtent, float yHalfExtent, float zHalfExtent);

public:
	inline Vec3 GetHalfExtents() const { return _halfExtents; }
	inline float GetXHalfExtent( ) const { return _halfExtents.x; }
	inline float GetYHalfExtent( ) const { return _halfExtents.y; }
	inline float GetZHalfExtent( ) const { return _halfExtents.z; }
	
	inline void SetHalfExtents(const Vec3& newHalfExtents) { _halfExtents = newHalfExtents; }
	inline void SetXHalfExtent(float newXHalf) { _halfExtents.x = newXHalf; }
	inline void SetYHalfExtent(float newYHalf) { _halfExtents.y = newYHalf; }
	inline void SetZHalfExtent(float newZHalf) { _halfExtents.y = newZHalf; }

private:
	Vec3 _halfExtents = { 1.f, 1.f, 1.f };
};

