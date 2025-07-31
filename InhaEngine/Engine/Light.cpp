#include "pch.h"
#include "Light.h"
#include "Scene.h"

Light::Light(Scene* scene, const LightInfo& info) : GameObject(scene)
{
	_lightIdx = -1;

	AddFixedComponent(new LightComponent(info));
}

shared_ptr<Light> Light::SpawnLight(Scene* scene, const LightInfo& info)
{
	Light* light = new Light(scene, info);
	shared_ptr<Light> ptr = scene->RegisterGameObject(light, false);
	light->SetLightIdx(scene->AddLight(ptr)); // TODO: Check if it's okay to remove SetLightIdx.
	return ptr;
}
