#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(Scene* scene, const LightInfo& info);
	DirectionalLight(DirectionalLight&&) = delete;

	static shared_ptr<DirectionalLight> SpawnDirectionalLight(Scene* scene, const LightInfo& info);
};

