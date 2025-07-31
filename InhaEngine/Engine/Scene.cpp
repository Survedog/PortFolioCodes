#include "pch.h"
#include "Scene.h"
#include "SceneRenderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "ShadowMapCamera.h"
#include "Light.h"
#include "Engine.h"
#include "Timer.h"
#include "FrameResource.h"
#include "Skybox.h"
#include "ResourceManager.h"
#include "GeometryGenerator.h"
#include "Material.h"

Scene::Scene() :
	Object(OBJECT_TYPE::SCENE),	
	_rootGameObject(nullptr),
	_ambientLightColor(Vec3::Zero),
	_sceneRenderer(make_shared<SceneRenderer>()), // TODO: Let SceneRenderer be set by something else. (make it not to be dependent to scene)	
	_skybox(make_shared<Skybox>())
{
	_mainCameraIdx = 0;
	_mainLightIdx = 0;		
}

void Scene::Load(const wstring& path)
{
	// TODO: Implement scene loading from file.
}

void Scene::InitializeGameObjects()
{	
	_rootGameObject = SpawnGameObject<GameObject>(); // TODO: Move it somewhere

	_rootGameObject->InitializeChildren();
}

void Scene::InitializeComponents()
{
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->InitializeComponents();
	}
}

void Scene::Awake()
{
	GetSkyBox()->SetMaterial(ResourceManager::GetInstance()->FindMaterial(L"Material_skybox"));
	GetSkyBox()->SetMesh(GeometryGenerator::CreateSphereMesh(100.f, 6U, 12U));

	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Awake();
	}
}

void Scene::Start()
{	
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Start();
	}
}

void Scene::Update(UPDATE_GROUP updateGroup)
{
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->Update(updateGroup);
	}
}

void Scene::FixedUpdate()
{
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		gameObject->FixedUpdate();
	}
}

void Scene::RenderScene()
{
	assert(GetSceneRenderer());
	GetSceneRenderer()->RenderScene(GetSharedPtr());
}

void Scene::FillGlobalData(GlobalConstants& outGlobalConstants) const
{
	for (size_t i = 0; i < _cameras.size(); ++i)
		if (_cameras[i] != nullptr)
		{
			outGlobalConstants.viewMatrices[i] = _cameras[i]->GetViewMatrix();
			outGlobalConstants.viewInvMatrices[i] = _cameras[i]->GetViewInverseMatrix();
			outGlobalConstants.projMatrices[i] = _cameras[i]->GetProjectionMatrix();
			outGlobalConstants.viewProjMatrices[i] = _cameras[i]->GetViewProjectionMatrix();
		}

	for (size_t i = 0; i < _shadowMapCameras.size(); ++i)
		if (_shadowMapCameras[i] != nullptr)
		{
			outGlobalConstants.shadowCameraViewProjMatrices[i] = _shadowMapCameras[i]->GetViewProjectionMatrix();
			outGlobalConstants.shadowMatrices[i] = _shadowMapCameras[i]->GetShadowMatrix();
		}

	outGlobalConstants.shadowCamera1_PartitionRange = _shadowMapCameras[0]->GetFrustumPartitionRange();
	outGlobalConstants.shadowCamera2_PartitionRange = _shadowMapCameras[1]->GetFrustumPartitionRange();
	outGlobalConstants.shadowCamera3_PartitionRange = _shadowMapCameras[2]->GetFrustumPartitionRange();

	outGlobalConstants.totalTime = ENGINE->GetGameTimer()->GetTotalTime();
	outGlobalConstants.deltaTime = ENGINE->GetGameTimer()->GetDeltaTime();	

	outGlobalConstants.ambientLightColor = GetAmbientLightColor();
	outGlobalConstants.lightCount = static_cast<uint32>(_lights.size());
	for (uint32 i = 0; i < outGlobalConstants.lightCount; ++i)
	{
		outGlobalConstants.lights[i] = _lights[i]->GetLightInfo();
		SwizzleVectorYZX(outGlobalConstants.lights[i].position);
		SwizzleVectorYZX(outGlobalConstants.lights[i].direction);
	}

	// TEMP
	outGlobalConstants.terrainRowScale = 1.f;
	outGlobalConstants.terrainColumnScale = 1.f;
	outGlobalConstants.terrainHeightScale = 1.f / 64.f;
}
