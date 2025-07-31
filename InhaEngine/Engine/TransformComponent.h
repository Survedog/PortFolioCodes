#pragma once

#include "Component.h"

class TransformComponent : public Component
{
public:
	TransformComponent();
	virtual ~TransformComponent() = default;	

public:
	shared_ptr<const TransformComponent> GetParentTransformComponent() const;
	shared_ptr<TransformComponent> GetParentTransformComponent();
	void GetDescendantTransformComponents(vector<shared_ptr<const TransformComponent>>& outDescendantTransformComponents) const;

	void PushData() const;

public:
	inline const Transform& GetLocalTransform() const { return _localTransform; }
	inline void SetLocalTransform(const Transform& newLocalTransform) { _localTransform = newLocalTransform; SetMatrixDirty(); }

	inline const Vec3& GetLocalPosition() const { return _localTransform.position; }
	inline const Quaternion& GetLocalRotation() const { return _localTransform.rotation; }
	inline const Vec3& GetLocalEulerRotation(bool isInRadian = false) const { return GetSwizzledVectorZXY(isInRadian ? _localTransform.rotation.ToEuler() : RadianVectorToDegree(_localTransform.rotation.ToEuler())); }
	inline const Vec3& GetLocalScale() const { return _localTransform.scale; }

	inline void SetLocalPosition(const Vec3& newPosition) { _localTransform.position = newPosition; SetMatrixDirty(); }
	inline void SetLocalRotation(const Vec3& newRotation, bool isInRadian = false)
	{
		Vec3 radianRotation = isInRadian ? newRotation : DegreeVectorToRadian(newRotation);
		SetLocalRotation(Quaternion::CreateFromYawPitchRoll(radianRotation.z, radianRotation.y, radianRotation.x));
	}
	inline void SetLocalRotation(const Quaternion& newRotation) { _localTransform.rotation = newRotation; SetMatrixDirty(); }
	inline void SetLocalScale(const Vec3& newScale) { _localTransform.scale = newScale; SetMatrixDirty(); }

	inline void GetWorldPosition(Vec3& outWorldPosition) const { outWorldPosition = GetSwizzledVectorZXY(GetWorldMatrix().Translation()); }
	inline Vec3 GetWorldPosition() const { return GetSwizzledVectorZXY(GetWorldMatrix().Translation()); }
	void SetWorldPosition(const Vec3& newWorldPosition);

	inline void RotateAround(const Vec3& axis, float angle, bool isInRadian = false)
	{
		// Swizzle axis vector to use directX coordinate system.
		Quaternion rotation = Quaternion::CreateFromAxisAngle(GetSwizzledVectorYZX(axis), isInRadian ? angle : XMConvertToRadians(angle));
		SetLocalRotation(GetLocalRotation() * rotation);
	}

public:
	// WARNING: Matrices are saved in DirectX coordinate system.(YZX)
	inline const Matrix& GetWorldMatrix() const { if (_isMatrixDirty) CalcTransformMatrix(); return _matWorld; }

	inline Vec3 GetRight() { return GetSwizzledVectorZXY(GetWorldMatrix().Right()); }
	inline Vec3 GetUp() { return GetSwizzledVectorZXY(GetWorldMatrix().Up()); }
	inline Vec3 GetForward() { return GetSwizzledVectorZXY(GetWorldMatrix().Backward()); }

private:
	void SetMatrixDirty() const;

	// Matrices are calculated in DirectX coordinate system.(YZX)
	void CalcTransformMatrix() const;

private:
	Transform _localTransform = {};
		
	mutable Matrix _matLocal; // Follows directX coordinate system.(YZX)
	mutable Matrix _matWorld; // Follows directX coordinate system.(YZX)

	mutable bool _isMatrixDirty;
};

