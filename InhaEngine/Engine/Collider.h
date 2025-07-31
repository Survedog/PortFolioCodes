#pragma once

enum class CollisionShape
{
	Box,
	Sphere,
	Capsule,
	Plane,
	HeightField,
	TriangleMesh
};

class Collider
{
public:
	Collider(CollisionShape shape);
	virtual ~Collider() = default;

public:
	inline CollisionShape GetCollisionShape() const { return _shape; }
	
	inline bool IsSimulated() const { return _isSimulated; }
	inline void SetSimulated(bool simulate) { _isSimulated = simulate; } // TODO: Implement simulation on/off functionality.

	inline Vec3 GetOffset() const { return _offset; }
	
	inline void SetOffset(const Vec3& newOffset) { _offset = newOffset; }
	inline void SetXOffset(float newX) { _offset.x = newX; }
	inline void SetYOffset(float newY) { _offset.y = newY; }
	inline void SetZOffset(float newZ) { _offset.z = newZ; }

private:
	CollisionShape _shape;
	bool _isSimulated = true; // TODO: Implement it.
	Vec3 _offset;
};
