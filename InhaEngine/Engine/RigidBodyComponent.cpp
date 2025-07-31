#include "pch.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "GameObject.h"

RigidBodyComponent::RigidBodyComponent() :
	Component(COMPONENT_TYPE::RIGIDBODY, UPDATE_GROUP::PrePhysics)
{
}

void RigidBodyComponent::OnOwnerGameObjectSet(GameObject* newOwnerGameObject)
{
	ENGINE->GetPhysicsManager()->RegisterGameObject(newOwnerGameObject->GetSharedPtr());
}

void RigidBodyComponent::SetGravityEnabled(bool enableGravity)
{
	_gravityEnabled = enableGravity;
	if (enableGravity != IsGravityEnabled())
		ENGINE->GetPhysicsManager()->SetGravityEnabled(GetOwnerGameObject(), enableGravity);
}
