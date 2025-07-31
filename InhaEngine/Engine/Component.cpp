#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(COMPONENT_TYPE componentType, UPDATE_GROUP updateGroup) :
	Object(OBJECT_TYPE::COMPONENT),
	_componentType(componentType),
	_updateEnabled(false),
	_updateGroup(UPDATE_GROUP::PrePhysics)
{
}

void Component::SetOwnerGameObject(GameObject* newOwnerGameObject)
{
	_ownerGameObject = newOwnerGameObject->GetWeakPtr();
	OnOwnerGameObjectSet(newOwnerGameObject);
}

void Component::SetUpdateGroup(UPDATE_GROUP updateGroup)
{
	_updateGroup = updateGroup;
	// TODO: put in update group vector
}

