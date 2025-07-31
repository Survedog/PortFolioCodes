#pragma once

#include "Object.h"

class GameObject;

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	TERRAIN,
	COLLIDER,
	RIGIDBODY,
	ANIMATOR,
	INPUT,
	// ...
	MONO_BEHAVIOUR,
	END
};


enum class UPDATE_GROUP : uint8
{
	PrePhysics, // Beginning of the Frame. Data is one frame-old.
	DuringPhysics, // Physics data can be updated or one frame-old.
	PostPhysics, // After physics simulation is completely over. Almost every gameobject's physics simulation is already over.
	Latent, // Adjust cameras
	PostUpdateWork, // The type of Works that needs final camera location can be done here.
	Final, // The type of works that need to be done in the last can be done here.
	END // Warning: Not an actual enum item!!
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END),
	UPDATE_GROUP_COUNT = static_cast<uint8>(UPDATE_GROUP::END)
};

class Component : public Object
{
public:
	Component(COMPONENT_TYPE componentType, UPDATE_GROUP updateGroup = UPDATE_GROUP::PrePhysics);
	virtual ~Component() = default;

public:
	inline COMPONENT_TYPE GetComponentType() const { return _componentType; }

	inline shared_ptr<GameObject> GetOwnerGameObject() { return _ownerGameObject.lock(); }
	inline shared_ptr<const GameObject> GetOwnerGameObject() const { return _ownerGameObject.lock(); }
	void SetOwnerGameObject(GameObject* newOwnerGameObject);

public:
	inline UPDATE_GROUP GetUpdateGroup() const { return _updateGroup; }
	void SetUpdateGroup(UPDATE_GROUP updateGroup);

	inline bool IsUpdateEnabled() const { return _updateEnabled; }
	// NOTE: Currently SetUpdateEnabled is only supported for calls on constructor.
	inline void SetUpdateEnabled(bool updateEnabled) { _updateEnabled = updateEnabled; }

protected:
	virtual void OnOwnerGameObjectSet(GameObject* newOwnerGameObject) {};

protected:
	friend class GameObject;
	virtual void Initialize(GameObject* ownerGameObject) { SetOwnerGameObject(ownerGameObject); }
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void OnDestroy() {}

private:
	COMPONENT_TYPE _componentType;

	weak_ptr<GameObject> _ownerGameObject;
	weak_ptr<Component> _parentComponent;
	
	UPDATE_GROUP _updateGroup;
	bool _updateEnabled;
};

