#include "pch.h"
#include "PhysicsManager.h"
#include "Engine.h"
#include "cooking/PxCooking.h"
#include "PhysXUtils.h"
#include "Timer.h"
#include "GameObject.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TerrainComponent.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "CapsuleCollider.h"

PhysicsManager::PhysicsManager() :
	_timer(make_shared<Timer>())
{
}

PhysicsManager::~PhysicsManager()
{
	//PxCloseExtensions();

	ReleaseResources();
}

void PhysicsManager::Init()
{
	ReleaseResources();

	// Foundation
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, GetAllocatorCallback(), GetErrorCallback());
	if (_foundation == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX Foundation.");

	// PVD
	_pvd = PxCreatePvd(*GetFoundation());
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	if (_pvd == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX PVD.");
	GetPVD()->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// Physics
	_toleranceScale.length = 100.f;
	_toleranceScale.speed = 980.f;
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *GetFoundation(), GetTolerancesScale(), ShouldRecordMemoryAllocations(), GetPVD());
	if (_physics == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX Physics.");
	// if (!PxInitExtensions(*GetPhysics(), GetPVD())) THROW_MSG_PHYSX(L"Failed to init PhysX extensions library.");

	// Dispatcher
	_dispatcher = PxDefaultCpuDispatcherCreate(_numThreads);
	if (_dispatcher == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX CPU Dispatcher.");

	// Scene
	PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.cpuDispatcher = GetCPUDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.gravity = PxVec3(0.f, -980.0f, 0.f);
	_scene = GetPhysics()->createScene(sceneDesc);
	if (_scene == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX Scene.");
	
	_pvdSceneClient = _scene->getScenePvdClient();
	if (_pvdSceneClient)
	{
		_pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		_pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		_pvdSceneClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	_defaultMaterial = GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	GetPhysicsTimer()->Reset();
	GetPhysicsTimer()->Start();
		
	_objectIdAggregateMap.clear();

	_elaspedTime = 0;
}

void PhysicsManager::ReleaseResources()
{
	_objectIdAggregateMap.clear();
	PX_RELEASE(_scene); _scene = nullptr;

	PX_RELEASE(_dispatcher); _dispatcher = nullptr; 
	PX_RELEASE(_material); _material = nullptr;
	PX_RELEASE(_physics); _physics = nullptr;

	if (_pvd != nullptr)
	{
		PxPvdTransport* transport = _pvd->getTransport();
		PX_RELEASE(_pvd); _pvd = nullptr; 
		PX_RELEASE(transport); transport = nullptr;
	}

	PX_RELEASE(_foundation); _foundation = nullptr;
}

void PhysicsManager::RegisterGameObject(shared_ptr<GameObject> object)
{
	if (object == nullptr) 
	{
		// TODO: Print error log.
		return;
	}

	// Return if the object is already registered.
	if (FindAggregate(object->GetID()) != nullptr) return;

	PxRigidActor* rigidActor = nullptr;
	PxVec3 position = GetPxCoordsFromGameCoords(object->GetWorldPosition());
	PxTransform pose = PxTransform(position, QuatToPxQuat(object->GetLocalRotation()));

	const bool enableSelfCollision = false;
	PxAggregateFilterHint hint = PxGetAggregateFilterHint(PxAggregateType::eSTATIC, enableSelfCollision);

	// Create physics actor.
	if (object->GetRigidBodyComponent() != nullptr) // Dynamic GameObject
	{
		hint = PxGetAggregateFilterHint(PxAggregateType::eGENERIC, enableSelfCollision);
		rigidActor = GetPhysics()->createRigidDynamic(pose);
	}
	else // Static GameObject
	{
		assert(object->IsStaticObject() && "GameObject must be static if it does not have a RigidBodyComponent.");
		rigidActor = GetPhysics()->createRigidStatic(pose);
	}
	
	if (rigidActor == nullptr) THROW_MSG_PHYSX(L"Failed to create PhysX Rigid Body.");

	// Add colliders to the actor.
	if (shared_ptr<ColliderComponent> colliderComp = object->GetColliderComponent())
		for (shared_ptr<Collider> collider : colliderComp->GetColliders())
			AttachPxShape(*rigidActor, collider);
	
	if (shared_ptr<TerrainComponent> terrainComp = object->GetTerrainComponent())
		AttachPxShape(*rigidActor, terrainComp->GetHeightMapCollider());

	// Synchronize rigidbody settings.
	if (shared_ptr<RigidBodyComponent> rigidBodyComp = object->GetRigidBodyComponent())
	{
		PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(rigidActor);
		LoadRigidBodyAttributesFromComponent(*rigidDynamic, rigidBodyComp);		
	}
	
	// Create aggregate and add rigid actor to it.
	PxAggregate* aggregate = GetPhysics()->createAggregate(1, GetActorMaxColliderCount(), hint);
	aggregate->addActor(*rigidActor);
	GetPhysXScene()->addAggregate(*aggregate);

	_objectIdAggregateMap.emplace(object->GetID(), aggregate);
	_aggregateObjectMap.emplace(aggregate, object);
}

void PhysicsManager::UnregisterGameObject(shared_ptr<GameObject> object)
{
	if (object == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	auto aggregateIter = _objectIdAggregateMap.find(object->GetID());
	if (aggregateIter != _objectIdAggregateMap.end())
	{
		PxAggregate* aggregate = aggregateIter->second;
		PX_RELEASE(aggregate);
		_objectIdAggregateMap.erase(aggregateIter);

		auto objIter = _aggregateObjectMap.find(aggregate);
		assert(objIter != _aggregateObjectMap.end());
		_aggregateObjectMap.erase(objIter);
	}
}

// TODO: Run this in a seperate thread.
bool PhysicsManager::SimulatePhysics()
{
	if (GetPhysicsTimer()->IsTimerStopped()) return false;

	GetPhysicsTimer()->Tick();
	_elaspedTime += GetPhysicsTimer()->GetDeltaTime();
	if (_elaspedTime >= _fixedTimeStep)
	{
		_elaspedTime -= _fixedTimeStep;

		GetPhysXScene()->simulate(_fixedTimeStep);
		if (!GetPhysXScene()->fetchResults(true))
		{
			// TODO: Print error log.
			return false;
		}
		return true;
	}

	return false;
}

// TODO: Make game data->physics data version too.
void PhysicsManager::SynchronizeGameData()
{
	uint32 numAggregates = GetPhysXScene()->getNbAggregates();
	vector<PxAggregate*> aggregates(numAggregates, nullptr);
	GetPhysXScene()->getAggregates(aggregates.data(), aggregates.size() * sizeof(PxAggregate*));

	for (PxAggregate* aggregate : aggregates)
		if (aggregate != nullptr)
		{
			shared_ptr<GameObject> gameObject = FindGameObject(aggregate);
			assert(gameObject != nullptr && "GameObject not found for the aggregate.");
			if (gameObject->IsStaticObject()) continue;
			
			PxActor* actor = GetFirstActor(aggregate);
			assert(actor != nullptr);

			if (actor->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC)
			{
				PxRigidBody* rigidBody = static_cast<PxRigidBody*>(actor);

				PxTransform actorTransform = rigidBody->getGlobalPose();
				gameObject->SetWorldPosition(GetGameCoordsFromPXCoords(actorTransform.p));
				gameObject->SetLocalRotation(PxQuatToQuat(actorTransform.q));

				SaveRigidBodyStateToComponent(*rigidBody, gameObject->GetRigidBodyComponent());
			}		
		}
}

// TODO: Implement logic for other shapes too.
void PhysicsManager::AttachPxShape(PxRigidActor& actor, shared_ptr<Collider> collider)
{
	if (collider == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxShape* shape = nullptr;
	if (collider->GetCollisionShape() == CollisionShape::Box)
	{
		shared_ptr<BoxCollider> boxCollider = static_pointer_cast<BoxCollider>(collider);
		PxVec3 halfExtents = GetPxCoordsFromGameCoords(boxCollider->GetHalfExtents());
		shape = GetPhysics()->createShape(PxBoxGeometry(halfExtents), *GetDefaultMaterial(), true);
	}
	else if (collider->GetCollisionShape() == CollisionShape::Sphere)
	{
		shared_ptr<SphereCollider> sphereCollider = static_pointer_cast<SphereCollider>(collider);
		shape = GetPhysics()->createShape(PxSphereGeometry(sphereCollider->GetRadius()), *GetDefaultMaterial(), true);
	}
	else if (collider->GetCollisionShape() == CollisionShape::Capsule)
	{
		shared_ptr<CapsuleCollider> capsuleCollider = static_pointer_cast<CapsuleCollider>(collider);
		float halfHeight = capsuleCollider->GetHalfHeight();
		float radius = capsuleCollider->GetRadius();
		shape = GetPhysics()->createShape(PxCapsuleGeometry(radius, halfHeight), *GetDefaultMaterial(), true);
	}
	else if (collider->GetCollisionShape() == CollisionShape::Plane)
	{
		if (actor.getConcreteType() != PxConcreteType::eRIGID_STATIC)
		{
			//TODO: Print error log.
			return;
		}
		shared_ptr<PlaneCollider> planeCollider = static_pointer_cast<PlaneCollider>(collider);
		PxVec3 normal = GetPxCoordsFromGameCoords(planeCollider->GetNormal());
		PxVec3 point = GetPxCoordsFromGameCoords(planeCollider->GetPoint());

		shape = GetPhysics()->createShape(PxPlaneGeometry(), *GetDefaultMaterial(), true);
		if (shape != nullptr) shape->setLocalPose(PxTransformFromPlaneEquation(PxPlane(point, normal)));
	}
	else if (collider->GetCollisionShape() == CollisionShape::HeightField)
	{
		shared_ptr<HeightMap> heightMap = static_pointer_cast<HeightMapCollider>(collider)->GetHeightMap();
		if (heightMap == nullptr)
		{
			//TODO: Print error log.
		}
		else
		{
			PxHeightFieldDesc heightFieldDesc;
			heightFieldDesc.nbColumns = heightMap->GetColumnCount();
			heightFieldDesc.nbRows = heightMap->GetRowCount();
			heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
			
			vector<HeightMapSample> heightFieldData;
			heightMap->GetPhysicsHeightFieldDataFromTexture(heightFieldData);

			heightFieldDesc.samples.data = reinterpret_cast<void*>(heightFieldData.data());
			heightFieldDesc.samples.stride = sizeof(HeightMapSample);
			heightFieldDesc.samples.count = heightMap->GetRowCount() * heightMap->GetColumnCount();

			if (PxHeightField* heightField = PxCreateHeightField(heightFieldDesc))
			{
				PxHeightFieldGeometry heightFieldGeometry(heightField);
				heightFieldGeometry.rowScale = heightMap->GetRowScale();
				heightFieldGeometry.columnScale = heightMap->GetColumnScale();
				heightFieldGeometry.heightScale = heightMap->GetHeightScale();
				shape = GetPhysics()->createShape(heightFieldGeometry, *GetDefaultMaterial());
			}
		}
	}
	else // if (collider->GetCollisionShape() == CollisionShape::TriangleMesh)
	{
		//PxRigidActorExt::createExclusiveShape(actor, PxTriangleMeshGeometry(), *GetDefaultMaterial());
	}

	if (shape == nullptr)
	{
		//Print Error log
		return;
	}

	PxVec3 shapeOffset = GetPxCoordsFromGameCoords(collider->GetOffset());
	shape->setLocalPose(shape->getLocalPose() * PxTransform(shapeOffset)); // TODO: Check if this is correct.
	shape->setRestOffset(0.01f);
	shape->setContactOffset(0.02f);

	actor.attachShape(*shape);
	shape->release();
}

void PhysicsManager::LoadRigidBodyAttributesFromComponent(PxRigidDynamic& rigidDynamic, shared_ptr<RigidBodyComponent> rigidBodyComp)
{
	if (rigidBodyComp == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	rigidDynamic.setMaxLinearVelocity(rigidBodyComp->GetMaxLinearVelocity());
	rigidDynamic.setMaxAngularVelocity(rigidBodyComp->GetMaxAngularVelocity());
	rigidDynamic.setLinearDamping(rigidBodyComp->GetLinearDamping());
	rigidDynamic.setAngularDamping(rigidBodyComp->GetAngularDamping());

	rigidDynamic.setSolverIterationCounts(rigidBodyComp->GetPositionSolverIters(), rigidBodyComp->GetVelocitySolverIters());
	rigidDynamic.setSleepThreshold(rigidBodyComp->GetSleepThreshold());
	rigidDynamic.setStabilizationThreshold(rigidBodyComp->GetStabilizationThreshold());

	rigidDynamic.setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !rigidBodyComp->IsGravityEnabled());
	
	if (rigidBodyComp->ShouldCalculateMassByDensity())
		PxRigidBodyExt::updateMassAndInertia(rigidDynamic, rigidBodyComp->GetDensity());
	else
		rigidDynamic.setMass(rigidBodyComp->GetMass());
}

void PhysicsManager::SaveRigidBodyStateToComponent(PxRigidBody& rigidBody, shared_ptr<RigidBodyComponent> rigidBodyComp)
{
	if (rigidBodyComp == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	rigidBodyComp->SetLinearVelocity(PxVec3ToVec3(rigidBody.getLinearVelocity()));
	rigidBodyComp->SetAngularVelocity(PxVec3ToVec3(rigidBody.getAngularVelocity()));
	rigidBodyComp->SetLinearAcceleration(PxVec3ToVec3(rigidBody.getLinearAcceleration()));
	rigidBodyComp->SetAngularAcceleration(PxVec3ToVec3(rigidBody.getAngularAcceleration()));
}

void PhysicsManager::AddCollider(shared_ptr<GameObject> object, shared_ptr<Collider> collider)
{
	if (object == nullptr || collider == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxActor* actor = FindMatchingActor(object->GetID());
	if (actor == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(actor);
	AttachPxShape(*rigidActor, collider);
}

void PhysicsManager::RemoveCollider(shared_ptr<GameObject> object, uint32 colliderIdx)
{
	if (object == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxActor* actor = FindMatchingActor(object->GetID());
	if (actor == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxRigidActor* rigidActor = static_cast<PxRigidActor*>(actor);

	PxShape* shapes[MAX_COLLIDER_PER_ACTOR];
	fill_n(shapes, MAX_COLLIDER_PER_ACTOR, nullptr);

	uint32 numShapes = rigidActor->getNbShapes(); // Get the number of shapes attached to the actor.
	rigidActor->getShapes(shapes, numShapes);
	for (uint32 i = 0; i < numShapes; ++i)
	{
		if (i == colliderIdx)
		{
			rigidActor->detachShape(*shapes[i]);
			shapes[i]->release();
			return;
		}
		// TODO: Check if other shapes also need to be released.
	}
}

void PhysicsManager::SetGravityEnabled(shared_ptr<GameObject> object, bool enableGravity)
{
	if (object == nullptr)
	{
		// TODO: Print error log.
		return;
	}

	PxAggregate* aggregate = FindAggregate(object->GetID());
	if (aggregate == nullptr) return;
	
	PxActor* actor = GetFirstActor(aggregate);
	assert(actor != nullptr && "Couldn't find the first actor of an aggregate");
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !enableGravity);
}
