#pragma once

#ifdef _DEBUG
	#pragma comment(lib, "PhysX\\debug\\LowLevel_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\LowLevelAABB_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\LowLevelDynamics_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysX_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXCharacterKinematic_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXCommon_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXCooking_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXExtensions_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXFoundation_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXPvdSDK_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXTask_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\PhysXVehicle2_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\SceneQuery_static_64.lib")
	#pragma comment(lib, "PhysX\\debug\\SimulationController_static_64.lib")
#else
	#pragma comment(lib, "PhysX\\checked\\LowLevel_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\LowLevelAABB_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\LowLevelDynamics_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysX_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXCharacterKinematic_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXCommon_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXCooking_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXExtensions_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXFoundation_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXPvdSDK_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXTask_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\PhysXVehicle2_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\SceneQuery_static_64.lib")
	#pragma comment(lib, "PhysX\\checked\\SimulationController_static_64.lib")
#endif // _DEBUG

#include "PxConfig.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"
#define MAX_COLLIDER_PER_ACTOR 5

using namespace physx;

class Timer;
class GameObject;
class Collider;
class ColliderComponent;
class RigidBodyComponent;

class PhysicsManager
{

public:
	PhysicsManager();
	~PhysicsManager(); // Note: This class is not for subclassing. (No virtual destructor)


	void Init();
	void ReleaseResources();
	void RegisterGameObject(shared_ptr<GameObject> object);
	void UnregisterGameObject(shared_ptr<GameObject> object);
	bool SimulatePhysics();

	void SynchronizeGameData();

public:
	void AddCollider(shared_ptr<GameObject> object, shared_ptr<Collider> collider);
	void RemoveCollider(shared_ptr<GameObject> object, uint32 colliderIdx);
	void SetGravityEnabled(shared_ptr<GameObject> object, bool enableGravity);

private:
	inline shared_ptr<GameObject> FindGameObject(PxAggregate* aggregate)
	{
		if (aggregate == nullptr) return nullptr;
		auto objectIter = _aggregateObjectMap.find(aggregate);
		return objectIter != _aggregateObjectMap.end() ? objectIter->second : nullptr;
	}

	inline PxAggregate* FindAggregate(uint32 objectId)
	{
		auto aggregateIter = _objectIdAggregateMap.find(objectId);
		return aggregateIter != _objectIdAggregateMap.end() ? aggregateIter->second : nullptr;
	}

	inline PxActor* GetFirstActor(PxAggregate* aggregate)
	{
		if (aggregate == nullptr || aggregate->getNbActors() == 0) return nullptr;
		PxActor* actor = nullptr;
		aggregate->getActors(&actor, sizeof(PxActor*));
		return actor;
	}

	inline PxActor* FindMatchingActor(uint32 objectId) { return GetFirstActor(FindAggregate(objectId)); }

	void AttachPxShape(PxRigidActor& actor, shared_ptr<Collider> collider);
	void LoadRigidBodyAttributesFromComponent(PxRigidDynamic& rigidDynamic, shared_ptr<RigidBodyComponent> rigidBodyComp);
	void SaveRigidBodyStateToComponent(PxRigidBody& rigidBody, shared_ptr<RigidBodyComponent> rigidBodyComp);

public:
	inline shared_ptr<Timer> GetPhysicsTimer() { return _timer; }
	inline shared_ptr<const Timer> GetPhysicsTimer() const { return _timer; }
	inline uint32 GetActorMaxColliderCount() const { return _aggregateMaxShape; }

private:
	inline PxAllocatorCallback& GetAllocatorCallback() { return _defaultAllocator; }
	inline const PxAllocatorCallback& GetAllocatorCallback() const { return _defaultAllocator; }

	inline PxErrorCallback& GetErrorCallback() { return _defaultErrorCallback; }
	inline const PxErrorCallback& GetErrorCallback() const { return _defaultErrorCallback; }

	inline PxTolerancesScale& GetTolerancesScale() { return _toleranceScale; }
	inline const PxTolerancesScale& GetTolerancesScale() const { return _toleranceScale; }

	inline PxFoundation* GetFoundation() { return _foundation; }
	inline const PxFoundation* GetFoundation() const { return _foundation; }

	inline PxPhysics* GetPhysics() { return _physics; }
	inline const PxPhysics* GetPhysics() const { return _physics; }

	inline PxMaterial* GetMaterial() { return _material; }
	inline const PxMaterial* GetMaterial() const { return _material; }

	inline PxScene* GetPhysXScene() { return _scene; }
	inline const PxScene* GetPhysXScene() const { return _scene; }

	inline PxPvd* GetPVD() { return _pvd; }
	inline const PxPvd* GetPVD() const { return _pvd; }

	inline PxCpuDispatcher* GetCPUDispatcher() { return _dispatcher; }
	inline const PxCpuDispatcher* GetCPUDispatcher() const { return _dispatcher; }

	inline PxMaterial* GetDefaultMaterial() { return _defaultMaterial; }
	inline const PxMaterial* GetDefaultMaterial() const { return _defaultMaterial; }

	inline bool ShouldRecordMemoryAllocations() const { return _recordMemoryAllocations; }

private:
	PxDefaultAllocator		_defaultAllocator;
	PxDefaultErrorCallback	_defaultErrorCallback;
	PxTolerancesScale		_toleranceScale;

	PxFoundation* _foundation = nullptr;
	PxPhysics* _physics = nullptr;
	PxMaterial* _material = nullptr;
	PxScene* _scene = nullptr;
	PxPvd* _pvd = nullptr;
	PxPvdSceneClient* _pvdSceneClient = nullptr;
	PxDefaultCpuDispatcher* _dispatcher = nullptr;
	PxMaterial* _defaultMaterial = nullptr; // TEMP

	bool _recordMemoryAllocations = true;
	PxU32 _numThreads = 2;
	
	const float _fixedTimeStep = 1.f / 60.f;
	float _elaspedTime = 0.f;
	shared_ptr<Timer> _timer;

	const uint32 _aggregateMaxShape = MAX_COLLIDER_PER_ACTOR;
	map<uint32, PxAggregate*> _objectIdAggregateMap;
	map<PxAggregate*, shared_ptr<GameObject>> _aggregateObjectMap;
};

