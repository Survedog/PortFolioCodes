#pragma once

#include "Object.h"
#include "Component.h"
#include "TransformComponent.h"
#include "MeshRenderer.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "LightComponent.h"
#include "ParticleSystemComponent.h"
#include "TerrainComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "Animator.h"

class Scene;

class GameObject : public Object, public enable_shared_from_this<GameObject>
{
public:
	GameObject(Scene* scene);
	virtual ~GameObject() = default;

	inline shared_ptr<GameObject> GetSharedPtr() { return shared_from_this(); }
	inline weak_ptr<GameObject> GetWeakPtr() { return weak_from_this(); }

public:
	inline shared_ptr<GameObject> GetParent() { return _parent.lock(); }
	inline shared_ptr<const GameObject> GetParent() const { return _parent.lock(); }

	inline const vector<shared_ptr<GameObject>>& GetChildren() { return _children; }
	inline void GetChildren(vector<shared_ptr<const GameObject>>& outChildren) const { outChildren.assign(_children.begin(), _children.end()); }

	// TODO: Check if it makes too much overhead.
	void GetDescendants(vector<shared_ptr<const GameObject>>& outDescendants, bool clearVector = true) const;	
	
	inline void SetParent(shared_ptr<GameObject> newParent) { _parent = newParent; if (newParent != nullptr) newParent->_children.push_back(GetSharedPtr()); }
	inline void AddChild(shared_ptr<GameObject> newChild) { newChild->SetParent(GetSharedPtr()); }

public:
	inline shared_ptr<Scene> GetScene() { return _scene.lock(); }
	inline shared_ptr<const Scene> GetScene() const { return _scene.lock(); }

public:
	inline const Transform& GetLocalTransform() const { return GetTransformComponent()->GetLocalTransform();	}

	inline const void GetWorldPosition(Vec3& outWorldPosition) const { GetTransformComponent()->GetWorldPosition(outWorldPosition); }
	inline const Vec3 GetWorldPosition() const { return GetTransformComponent()->GetWorldPosition(); }
	inline const Vec3& GetLocalPosition() const { return GetLocalTransform().position; }
	inline const Quaternion& GetLocalRotation() const { return GetLocalTransform().rotation; } // Engine coordinate system is Z-up and DirectX is Y-up. Check if it makes any difference on quaternion.
	inline const Vec3& GetLocalEulerRotation(bool isInRadian = false) const { return GetTransformComponent()->GetLocalEulerRotation(isInRadian); }
	inline const Vec3& GetLocalScale() const { return GetLocalTransform().scale; }

	inline void SetWorldPosition(const Vec3& newPosition) { GetTransformComponent()->SetWorldPosition(newPosition);	}
	inline void SetLocalPosition(const Vec3& newPosition) { GetTransformComponent()->SetLocalPosition(newPosition); }
	// TODO: Apply circular clamping to angle
	inline void SetLocalRotation(const Quaternion& newRotation) { GetTransformComponent()->SetLocalRotation(newRotation); }
	inline void SetLocalRotation(const Vec3& newRotation, bool isInRadian = false) { GetTransformComponent()->SetLocalRotation(newRotation, isInRadian); }
	inline void SetLocalScale(const Vec3& newScale) { GetTransformComponent()->SetLocalScale(newScale); }

public:
	inline shared_ptr<const TransformComponent> GetTransformComponent() const { return static_pointer_cast<const TransformComponent>(GetFixedComponent(COMPONENT_TYPE::TRANSFORM)); }
	inline shared_ptr<TransformComponent> GetTransformComponent() { return static_pointer_cast<TransformComponent>(GetFixedComponent(COMPONENT_TYPE::TRANSFORM)); }

	inline shared_ptr<const MeshRenderer> GetMeshRenderer() const { return static_pointer_cast<const MeshRenderer>(GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER)); }
	inline shared_ptr<MeshRenderer> GetMeshRenderer() { return static_pointer_cast<MeshRenderer>(GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER));	}

	inline shared_ptr<const CameraComponent> GetCameraComponent() const { return static_pointer_cast<const CameraComponent>(GetFixedComponent(COMPONENT_TYPE::CAMERA)); }
	inline shared_ptr<CameraComponent> GetCameraComponent() { return static_pointer_cast<CameraComponent>(GetFixedComponent(COMPONENT_TYPE::CAMERA)); }

	inline shared_ptr<const InputComponent> GetInputComponent() const { return static_pointer_cast<const InputComponent>(GetFixedComponent(COMPONENT_TYPE::INPUT)); }
	inline shared_ptr<InputComponent> GetInputComponent() { return static_pointer_cast<InputComponent>(GetFixedComponent(COMPONENT_TYPE::INPUT)); }

	inline shared_ptr<const LightComponent> GetLightComponent() const { return static_pointer_cast<const LightComponent>(GetFixedComponent(COMPONENT_TYPE::LIGHT)); }
	inline shared_ptr<LightComponent> GetLightComponent() { return static_pointer_cast<LightComponent>(GetFixedComponent(COMPONENT_TYPE::LIGHT)); }

	inline shared_ptr<const ParticleSystemComponent> GetParticleSystemComponent() const { return static_pointer_cast<const ParticleSystemComponent>(GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)); }
	inline shared_ptr<ParticleSystemComponent> GetParticleSystemComponent() { return static_pointer_cast<ParticleSystemComponent>(GetFixedComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)); }

	inline shared_ptr<const TerrainComponent> GetTerrainComponent() const { return static_pointer_cast<const TerrainComponent>(GetFixedComponent(COMPONENT_TYPE::TERRAIN)); }
	inline shared_ptr<TerrainComponent> GetTerrainComponent() { return static_pointer_cast<TerrainComponent>(GetFixedComponent(COMPONENT_TYPE::TERRAIN)); }

	inline shared_ptr<const ColliderComponent> GetColliderComponent() const { return static_pointer_cast<const ColliderComponent>(GetFixedComponent(COMPONENT_TYPE::COLLIDER)); }
	inline shared_ptr<ColliderComponent> GetColliderComponent() { return static_pointer_cast<ColliderComponent>(GetFixedComponent(COMPONENT_TYPE::COLLIDER)); }

	inline shared_ptr<const RigidBodyComponent> GetRigidBodyComponent() const { return static_pointer_cast<const RigidBodyComponent>(GetFixedComponent(COMPONENT_TYPE::RIGIDBODY)); }
	inline shared_ptr<RigidBodyComponent> GetRigidBodyComponent() { return static_pointer_cast<RigidBodyComponent>(GetFixedComponent(COMPONENT_TYPE::RIGIDBODY)); }

	inline shared_ptr<const Animator> GetAnimator() const { return static_pointer_cast<const Animator>(GetFixedComponent(COMPONENT_TYPE::ANIMATOR)); }
	inline shared_ptr<Animator> GetAnimator() { return static_pointer_cast<Animator>(GetFixedComponent(COMPONENT_TYPE::ANIMATOR)); }

	// TODO: Make it return-by-value.
	inline const shared_ptr<Component>& GetFixedComponent(COMPONENT_TYPE type) const
	{
		uint8 typeIdx = static_cast<uint8>(type);
		assert(typeIdx < _countof(_fixedComponents));
		return _fixedComponents[typeIdx];
	}

	inline shared_ptr<Component>& GetFixedComponent(COMPONENT_TYPE type)
	{
		uint8 typeIdx = static_cast<uint8>(type);
		assert(typeIdx < _countof(_fixedComponents));
		return _fixedComponents[typeIdx];
	}

	// WARNING: The argument component must not be managed by smart pointer yet.
	void AddFixedComponent(Component* newComponent);

public:
	inline bool IsStaticObject() const { return _isStaticObject; }
	inline void SetStaticObject(bool isStaticObject) { _isStaticObject = isStaticObject; }

protected:
	inline vector<Component*>& GetComponentGroup(UPDATE_GROUP updateGroup) { return _componentGroup[static_cast<uint8>(updateGroup)]; }	

protected:
	friend class Scene;
	virtual void InitializeChildren();
	virtual void InitializeComponents();
	virtual void Awake();
	virtual void Start();
	virtual void Update(UPDATE_GROUP updateGroup);
	virtual void FixedUpdate();
	virtual void OnDestroy(); // TODO: Call it when the object is deleted or scene is destroyed.

private:
	shared_ptr<Component> _fixedComponents[FIXED_COMPONENT_COUNT] = {};

	// NOTE: Must be used only internally.
	vector<Component*> _componentGroup[UPDATE_GROUP_COUNT];

	weak_ptr<GameObject> _parent;
	vector<shared_ptr<GameObject>> _children;

	weak_ptr<Scene> _scene;	
	bool _isStaticObject = false;	
};

