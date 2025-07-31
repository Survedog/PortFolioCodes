#pragma once

#include "Object.h"
#include "Component.h"
#include "GameObject.h"
#include "DirectionalLight.h"
#include "ShadowMapCamera.h"

class SceneRenderer;
class Camera;
class CameraComponent;
class ShadowMapCamera;
class Light;
class LightComponent;
class Skybox;

enum class ActiveSceneState
{
	CREATED,
	INITIATED,
	AWAKEN,
	STARTED,
	ENDPLAY,
	DESTROYED
};

class Scene : public Object, public enable_shared_from_this<Scene>
{	
public:
	Scene();
	virtual ~Scene() = default;

	virtual void Load(const wstring& path) override;

	inline shared_ptr<Scene> GetSharedPtr() { return shared_from_this(); }
	inline weak_ptr<Scene> GetWeakPtr() { return weak_from_this(); }

private:
	friend class Engine;
	void InitializeGameObjects();
	void InitializeComponents();
	void Awake();
	void Start();
	void Update(UPDATE_GROUP updateGroup);
	void FixedUpdate();

// Rendering
public:
	void RenderScene();

	inline shared_ptr<SceneRenderer> GetSceneRenderer() { return _sceneRenderer; }
	inline void SetSceneRenderer(shared_ptr<SceneRenderer> newSceneRenderer) { _sceneRenderer = newSceneRenderer; }

private:
	shared_ptr<SceneRenderer> _sceneRenderer;

// GameObject Management
public:
	inline shared_ptr<GameObject> GetRootGameObject() { return _rootGameObject; }
	inline const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	inline shared_ptr<GameObject> SpawnGameObject() { return SpawnGameObject<GameObject>(); }

	// NOTE: Should be called after construction finished.
	// TODO: Trigger lifecycle functions(like Awake, Start) on gameobjects that's spawned after those life stages of the scene.
	template<typename T>
	shared_ptr<T> SpawnGameObject() // TODO: Will shared pointer cause too much overhead?
	{
		static_assert(is_base_of_v<GameObject, T>);
		shared_ptr<T> spawned = make_shared<T>(this);
		_gameObjects.emplace_back(spawned);
		spawned->SetParent(_rootGameObject);
		return spawned;
	}

	template<typename T>
	shared_ptr<T> RegisterGameObject(T* gameObject, bool isManagedBySharedPtr)
	{
		static_assert(is_base_of_v<GameObject, T>);

		shared_ptr<T> ptr = nullptr;
		if (isManagedBySharedPtr)
			ptr = static_pointer_cast<T>(gameObject->GetSharedPtr());
		else
			ptr.reset(gameObject);

		_gameObjects.emplace_back(ptr);
		ptr->SetParent(_rootGameObject);
		return ptr;
	}

private:
	shared_ptr<GameObject> _rootGameObject;
	vector<shared_ptr<GameObject>> _gameObjects; // TODO: Change Data Structure

// Camera & ShadowMap
public:
	inline shared_ptr<Camera> GetMainCamera() {	return const_pointer_cast<Camera>(CONST_THIS->GetMainCamera()); }
	inline shared_ptr<const Camera> GetMainCamera() const
	{
		assert(_mainCameraIdx >= 0 && _mainCameraIdx < _cameras.size());
		return _cameras[_mainCameraIdx];
	}

	inline vector<shared_ptr<Camera>>& GetCameras() { return _cameras; }
	inline void GetCameras(vector<shared_ptr<const Camera>>& outCameras) const { outCameras.assign(_cameras.begin(), _cameras.end()); }

	// Returns camera's index on the camera vector of the scene.
	inline int32 AddCamera(shared_ptr<Camera> newCamera)
	{
		assert(_cameras.size() < MAX_CAMERAS && "The number of cameras has exceeded the max limit.");
		_cameras.push_back(newCamera);
		newCamera->SetCameraIdx(static_cast<int32>(_cameras.size()) - 1);
		return newCamera->GetCameraIdx();
	}

	inline void RemoveCamera(int32 cameraIdx)
	{
		if (cameraIdx == _mainCameraIdx) { /*Put warning message.*/ return; }
		assert(cameraIdx >= 0 && cameraIdx < static_cast<int32>(_cameras.size()));
		_cameras.erase(_cameras.begin() + cameraIdx);
		UpdateCameraIndices();
	}

	inline void SetMainCamera(shared_ptr<Camera> newCamera) { SetMainCamera(AddCamera(newCamera)); }
	inline void SetMainCamera(int32 newMainCameraIdx) { _mainCameraIdx = newMainCameraIdx; }

	inline shared_ptr<ShadowMapCamera> GetShadowMapCamera(uint32 idx) { return const_pointer_cast<ShadowMapCamera>(CONST_THIS->GetShadowMapCamera(idx)); }
	inline shared_ptr<const ShadowMapCamera> GetShadowMapCamera(uint32 idx) const
	{
		if (idx >= _shadowMapCameras.size()) return nullptr;
		return _shadowMapCameras[idx];
	}

	inline vector<shared_ptr<ShadowMapCamera>>& GetShadowMapCameras() { return _shadowMapCameras; }
	inline void GetShadowMapCameras(vector<shared_ptr<const ShadowMapCamera>>& outCameras) const { outCameras.assign(_shadowMapCameras.begin(), _shadowMapCameras.end()); }

	// Returns shadow map camera's camera index
	inline int32 AddShadowMapCamera(shared_ptr<ShadowMapCamera> newCamera)
	{
		assert(_shadowMapCameras.size() < MAX_SHADOW_MAP_CAMERAS, "The number of shadow map cameras has exceeded the max limit.");
		_shadowMapCameras.push_back(newCamera);
		newCamera->SetCameraIdx(static_cast<int32>(_shadowMapCameras.size()) - 1);
		return newCamera->GetCameraIdx();
	}

	inline void RemoveShadowMapCamera(int32 cameraIdx)
	{
		assert(cameraIdx >= 0 && cameraIdx < static_cast<int32>(_shadowMapCameras.size()));
		_shadowMapCameras.erase(_shadowMapCameras.begin() + cameraIdx);
		UpdateShadowMapCameraIndices();
	}

private:
	inline void UpdateCameraIndices()
	{
		for (uint32 idx = 0; idx < _cameras.size(); ++idx)
		{
			if (_cameras[idx]->GetCameraIdx() == _mainCameraIdx) _mainCameraIdx = idx;
			_cameras[idx]->SetCameraIdx(idx);
		}
	}

	inline void UpdateShadowMapCameraIndices()
	{
		for (uint32 idx = 0; idx < _shadowMapCameras.size(); ++idx)
			_shadowMapCameras[idx]->SetCameraIdx(idx);
	}

private:	
	vector<shared_ptr<Camera>> _cameras; // Shadow map camera will not be managed by this.
	vector<shared_ptr<ShadowMapCamera>> _shadowMapCameras;
	int32 _mainCameraIdx;


// Light
public:
	// Returns light's index on the camera vector of the scene.
	inline int32 AddLight(shared_ptr<Light> newLight)
	{
		_lights.push_back(newLight);
		newLight->SetLightIdx(static_cast<int32>(_lights.size()) - 1);
		return newLight->GetLightIdx();
	}

	inline void RemoveLight(int32 lightIdx)
	{
		if (lightIdx == _mainLightIdx) { /*Put warning message.*/ return; }
		assert(lightIdx >= 0 && lightIdx < static_cast<int32>(_lights.size()));
		_lights.erase(_lights.begin() + lightIdx);
		UpdateLightIndices();
	}

	inline Vec3 GetAmbientLightColor() const { return _ambientLightColor; }

	inline const vector<shared_ptr<Light>>& GetLights() { return _lights; }
	inline void GetLights(vector<shared_ptr<const Light>>& outLights) const { outLights.assign(_lights.begin(), _lights.end()); }

	inline shared_ptr<DirectionalLight> GetMainLight() { return const_pointer_cast<DirectionalLight>(CONST_THIS->GetMainLight()); }
	inline shared_ptr<const DirectionalLight> GetMainLight() const
	{
		if (_mainLightIdx < 0 || _mainLightIdx >= static_cast<int32>(_lights.size())) return nullptr;
		return static_pointer_cast<DirectionalLight>(_lights[_mainLightIdx]);
	}

	inline void SetAmbientLightColor(Vec3 newColor) { _ambientLightColor = newColor; }

	// WARNING: The input light must not be managed (Added) by the scene yet.
	inline void SetMainLight(shared_ptr<DirectionalLight> newMainLight) { SetMainLight(AddLight(newMainLight)); }
	inline void SetMainLight(int32 newMainLightIdx)
	{
		if (newMainLightIdx < 0 || newMainLightIdx >= static_cast<int32>(_lights.size())
			|| _lights[newMainLightIdx]->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
		{
			// TODO: Put warning message.
			return;
		}
		_mainLightIdx = newMainLightIdx;
	}

	void FillGlobalData(struct GlobalConstants& outGlobalConstants) const;

private:
	inline void UpdateLightIndices()
	{
		for (uint32 idx = 0; idx < _lights.size(); ++idx)
		{
			if (_lights[idx]->GetLightIdx() == _mainLightIdx) _mainLightIdx = idx;
			_lights[idx]->SetLightIdx(idx);
		}
	}

private:
	// The light at index 0 must be a directional light. (It is spawned and managed by the scene)
	vector<shared_ptr<Light>> _lights;
	int32 _mainLightIdx;
	Vec3 _ambientLightColor;

// Skybox
public:
	shared_ptr<Skybox> GetSkyBox() { return _skybox; }
	shared_ptr<const Skybox> GetSkyBox() const { return _skybox; }

private:
	shared_ptr<Skybox> _skybox;
};
