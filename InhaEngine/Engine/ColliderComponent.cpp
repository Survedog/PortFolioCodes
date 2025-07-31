#include "pch.h"
#include "ColliderComponent.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "BoxCollider.h"

ColliderComponent::ColliderComponent() :
	Component(COMPONENT_TYPE::COLLIDER, UPDATE_GROUP::DuringPhysics)
{
}

void ColliderComponent::AddCollider(shared_ptr<Collider> collider)
{	
	if (collider == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	if (_colliders.size() < ENGINE->GetPhysicsManager()->GetActorMaxColliderCount())
	{
		_colliders.push_back(collider);
		if (GetOwnerGameObject() != nullptr)
		{
			ENGINE->GetPhysicsManager()->AddCollider(GetOwnerGameObject(), collider);
		}
	}
	else
	{
		// TODO: Print error log.
	}
}

void ColliderComponent::RemoveCollider(uint32 colliderIdx)
{
	if (colliderIdx < _colliders.size())
	{
		_colliders.erase(_colliders.begin() + colliderIdx);
		ENGINE->GetPhysicsManager()->RemoveCollider(GetOwnerGameObject(), colliderIdx);
	}
	else
	{
		// TODO: Print error log.
	}
}

void ColliderComponent::OnOwnerGameObjectSet(GameObject* newOwnerGameObject)
{
	ENGINE->GetPhysicsManager()->RegisterGameObject(newOwnerGameObject->GetSharedPtr());
}