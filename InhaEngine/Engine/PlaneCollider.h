#pragma once
#include "Collider.h"
class PlaneCollider : public Collider
{
public:
	PlaneCollider(Vec3 normal, Vec3 point);
	PlaneCollider(float normalX, float normalY, float normalZ, Vec3 point);
	PlaneCollider(Vec3 normal, float pointX, float pointY, float pointZ);
	PlaneCollider(float normalX, float normalY, float normalZ, float pointX, float pointY, float pointZ);

public:
	inline Vec3 GetNormal() const { return _normal; }
	
	inline Vec3 GetPoint() const { return _point; }
	inline float GetPointX() const { return _point.x; }
	inline float GetPointY() const { return _point.y; }
	inline float GetPointZ() const { return _point.z; }

	// A plane need to be a static object.
	//inline void SetNormal(Vec3 newNormal) { _normal = newNormal; }
	//
	//inline void SetPoint(Vec3 newPoint) { _point = newPoint; }
	//inline void SetPointX(float newPointX) { _point.x = newPointX; }
	//inline void SetPointY(float newPointY) { _point.y = newPointY; }
	//inline void SetPointZ(float newPointZ) { _point.z = newPointZ; }

private:
	Vec3 _normal = { 0.f, 0.f, 1.f };
	Vec3 _point = { 0.f, 0.f, 0.f };
};

