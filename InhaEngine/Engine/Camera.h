#pragma once

#include "GameObject.h"
#include "CameraComponent.h"

// TODO: Remove camera on scene when destroyed.
class Camera : public GameObject
{
public:
	// WARNING: Do not construct camera directly. Use SpawnCamera method.
	Camera(Scene* scene);

	// WARNING: Do not construct camera directly. Use SpawnCamera method.
	Camera(Scene* scene, CameraComponent* cameraComponent);

	static shared_ptr<Camera> SpawnCamera(Scene* scene);

public:
	inline void Render() { GetCameraComponent()->Render(); }
	inline void SortGameObjects() { GetCameraComponent()->SortGameObjects(); }

	void FillCameraData(struct PerPassConstants& outPassConstants) const;

public:
	inline PROJECTION_TYPE GetProjectionType() const { return GetCameraComponent()->GetProjectionType(); }
	inline float GetNearZ() const { return GetCameraComponent()->GetNearZ(); }
	inline float GetFarZ() const { return GetCameraComponent()->GetFarZ(); }
	inline float GetFOV() const { return GetCameraComponent()->GetFOV(); }
	inline float GetWidth() const { return GetCameraComponent()->GetWidth(); }
	inline float GetHeight() const { return GetCameraComponent()->GetHeight(); }

	inline void SetProjectionType(PROJECTION_TYPE type) { GetCameraComponent()->SetProjectionType(type); }
	inline void SetNearZ(float newNearZ) { GetCameraComponent()->SetNearZ(newNearZ); }
	inline void SetFarZ(float newFarZ) { GetCameraComponent()->SetFarZ(newFarZ); }
	inline void SetFOV(float newFov) { GetCameraComponent()->SetFOV(newFov); }
	inline void SetWidth(float newWidth) { GetCameraComponent()->SetWidth(newWidth); }
	inline void SetHeight(float newHeight) { GetCameraComponent()->SetHeight(newHeight); }

public:
	const Matrix& GetViewMatrix() const { return GetCameraComponent()->GetViewMatrix(); }
	const Matrix& GetViewInverseMatrix() const { return GetCameraComponent()->GetViewInverseMatrix(); }
	const Matrix& GetProjectionMatrix() const { return GetCameraComponent()->GetProjectionMatrix(); }
	const Matrix& GetViewProjectionMatrix() const { return GetCameraComponent()->GetViewProjectionMatrix(); }

	inline int32 GetCameraIdx() const { return _cameraIdx; }
	inline void SetCameraIdx(int32 newCameraIdx) { _cameraIdx = newCameraIdx; }

private:
	int32 _cameraIdx = -1;
};

