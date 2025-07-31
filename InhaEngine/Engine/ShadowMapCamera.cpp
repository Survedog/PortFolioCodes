#include "pch.h"
#include "ShadowMapCamera.h"
#include "Texture.h"
#include "Scene.h"
#include "DirectionalLight.h"

// NOTE: The nearZ, farZ, width, height of a shadow map camera will be adjusted
// automatically according to the main camera's frustum.
ShadowMapCamera::ShadowMapCamera(Scene* scene) :
	Camera(scene, new ShadowMapCameraComponent())
{	
}

shared_ptr<ShadowMapCamera> ShadowMapCamera::SpawnShadowMapCamera(Scene* scene)
{
	ShadowMapCamera* camera = new ShadowMapCamera(scene);
	shared_ptr<ShadowMapCamera> ptr = scene->RegisterGameObject(camera, false);
	camera->SetCameraIdx(scene->AddShadowMapCamera(ptr));
	return ptr;
}
