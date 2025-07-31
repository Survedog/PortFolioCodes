#include "pch.h"
#include "DirectionalLight.h"
#include "ShadowMapCamera.h"
#include "Scene.h"

DirectionalLight::DirectionalLight(Scene* scene, const LightInfo& info) :
	Light(scene, info)
{
	if (info.lightType != LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		// TODO: Make error or warning message.
		GetLightComponent()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	}
}

shared_ptr<DirectionalLight> DirectionalLight::SpawnDirectionalLight(Scene* scene, const LightInfo& info)
{
	DirectionalLight* light = new DirectionalLight(scene, info);
	shared_ptr<DirectionalLight> ptr = scene->RegisterGameObject(light, false);

	scene->AddLight(ptr);
	return ptr;
}
