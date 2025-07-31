#include "pch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Engine.h"
#include "PhysicsManager.h"

GameObject::GameObject(Scene* scene) :
    Object(OBJECT_TYPE::GAMEOBJECT),
    _scene(scene->GetWeakPtr())
{
    AddFixedComponent(new TransformComponent());
}

void GameObject::GetDescendants(vector<shared_ptr<const GameObject>>& outDescendants, bool clearVector) const
{
	if (_children.empty()) return;

	if (clearVector) outDescendants.clear();
	outDescendants.insert(outDescendants.begin(), _children.begin(), _children.end());

	for (const shared_ptr<GameObject>& child : _children)
		child->GetDescendants(outDescendants, false);
}

// TODO: Implement COMPONENT_TYPE parameter version using factory function callback.
void GameObject::AddFixedComponent(Component* newComponent)
{
    assert(newComponent != nullptr && GetFixedComponent(newComponent->GetComponentType()) == nullptr);
    GetFixedComponent(newComponent->GetComponentType()).reset(newComponent);

    if (newComponent->IsUpdateEnabled())
    {
        GetComponentGroup(newComponent->GetUpdateGroup()).push_back(newComponent);
    }
}

void GameObject::InitializeChildren()
{
    for (shared_ptr<GameObject>& child : _children)
        if (child != nullptr)
            child->InitializeChildren();
}

void GameObject::InitializeComponents()
{
    for (shared_ptr<Component>& component : _fixedComponents)
        if (component != nullptr)
            component->Initialize(this);
}

void GameObject::Awake()
{
    for (shared_ptr<Component>& component : _fixedComponents)
        if (component != nullptr)
            component->Awake();
}

void GameObject::Start()
{
    for (shared_ptr<Component>& component : _fixedComponents)
        if (component != nullptr)
            component->Start();
}

void GameObject::Update(UPDATE_GROUP updateGroup)
{
    for (Component* component : GetComponentGroup(updateGroup)) // TODO: Check if the whole vector is copied (by reading assembly)
        if (component != nullptr)
            component->Update();
}

void GameObject::FixedUpdate()
{
	for (shared_ptr<Component> component : _fixedComponents) // TODO: Check if the whole vector is copied (by reading assembly)
		if (component != nullptr)
			component->FixedUpdate();
}

void GameObject::OnDestroy()
{
	ENGINE->GetPhysicsManager()->UnregisterGameObject(GetSharedPtr());

	for (shared_ptr<Component> component : _fixedComponents) // TODO: Check if the whole vector is copied (by reading assembly)
		if (component != nullptr)
			component->OnDestroy();	
}
