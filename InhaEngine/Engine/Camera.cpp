#include "pch.h"
#include "Camera.h"
#include "Scene.h"
#include "FrameResource.h"

Camera::Camera(Scene* scene) :
	GameObject(scene)
{
	AddFixedComponent(new CameraComponent());
}

// NOTE: cameraComponent must not be nullptr.
Camera::Camera(Scene* scene, CameraComponent* cameraComponent)
	: GameObject(scene)
{
	assert(cameraComponent != nullptr);
	AddFixedComponent(cameraComponent);
}

shared_ptr<Camera> Camera::SpawnCamera(Scene* scene)
{
	Camera* camera = new Camera(scene);
	shared_ptr<Camera> ptr = scene->RegisterGameObject(camera, false);
	camera->SetCameraIdx(scene->AddCamera(ptr));
	return ptr;
}

void Camera::FillCameraData(PerPassConstants& outPassConstants) const
{
	outPassConstants.currentCameraIdx = GetCameraIdx();
	GetCameraComponent()->FillCameraData(outPassConstants);
}
