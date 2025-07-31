#pragma once

#include "Component.h"

enum PROJECTION_TYPE : uint32
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class CameraComponent : public Component
{
public:
	CameraComponent();
	CameraComponent(UPDATE_GROUP updateGroup);
	virtual ~CameraComponent() = default;

protected:
	virtual void Update() override;

public:
	void Render();
	void SortGameObjects();

	void FillCameraData(struct PerPassConstants& outPassConstants) const;

	inline void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	inline bool IsLayerCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }

protected:
	// Only for temporary use. (objects are not managed by shared pointer)
	inline void GetForwardRenderObjects(vector<GameObject*>& outObjects) { outObjects.assign(_forwardRenderObjects.begin(), _forwardRenderObjects.end()); }

private:
	uint32 _cullingMask = 0;
	vector<GameObject*> _forwardRenderObjects; // TODO: Is it okay to use just raw pointer?

public:
	inline const Matrix& GetViewMatrix() const { return _matView; }
	const Matrix& GetViewInverseMatrix() const;
	inline const Matrix& GetProjectionMatrix() const { return _matProjection; }
	inline const Matrix& GetViewProjectionMatrix() const { return _matViewProjection; }

public:
	inline PROJECTION_TYPE GetProjectionType() const { return _projectionType; }
	inline float GetNearZ() const { return _nearZ; }
	inline float GetFarZ() const { return _farZ; }
	inline float GetFOV() const { return _fov; }
	inline float GetWidth() const { return _width; }
	inline float GetHeight() const { return _height; }

	inline void SetProjectionType(PROJECTION_TYPE type) { _projectionType = type; }
	inline void SetNearZ(float newNearZ) { _nearZ = newNearZ; }
	inline void SetFarZ(float newFarZ) { _farZ = newFarZ; }
	inline void SetFOV(float newFov) { _fov = newFov; }
	inline void SetWidth(float newWidth) { _width = newWidth; }
	inline void SetHeight(float newHeight) { _height = newHeight; }

protected:
	inline void SetViewMatrix(const Matrix& newViewMatrix) { _matView = newViewMatrix; }
	inline void SetProjectionMatrix(const Matrix& newProjectionMatrix) { _matProjection = newProjectionMatrix; }
	inline void SetViewProjectionMatrix(const Matrix& newViewProjectionMatrix) { _matViewProjection = newViewProjectionMatrix; }

private:
	PROJECTION_TYPE _projectionType;

	float _nearZ;
	float _farZ;
	float _fov;
	float _width;
	float _height;

	Matrix _matView;
	Matrix _matProjection;
	Matrix _matViewProjection;
};

