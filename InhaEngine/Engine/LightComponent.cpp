#include "pch.h"
#include "LightComponent.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "GameObject.h"

LightComponent::LightComponent(LightInfo info) :
	Component(COMPONENT_TYPE::LIGHT, UPDATE_GROUP::PostUpdateWork),
	_lightInfo(info)
{	
	SetUpdateEnabled(true);
}

void LightComponent::Update()
{
	assert(GetOwnerGameObject() != nullptr);
	SetLightPosition(GetOwnerGameObject()->GetWorldPosition());
	SetLightDirection(GetOwnerGameObject()->GetTransformComponent()->GetForward());
}
