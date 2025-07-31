#include "pch.h"
#include "CameraComponent.h"
#include "MeshRenderer.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Scene.h"
#include "Skybox.h"
#include "Engine.h"

CameraComponent::CameraComponent() :
	Component(COMPONENT_TYPE::CAMERA, UPDATE_GROUP::Latent),
	_matView(Matrix::Identity),
	_matProjection(Matrix::Identity),
	_matViewProjection(Matrix::Identity)
{
	SetUpdateEnabled(true);

	_nearZ = 1.f;
	_farZ = 5000.f;
	_fov = XM_PI / 4.f;
	_width = static_cast<float>(ENGINE->GetWindowInfo().width);
	_height = static_cast<float>(ENGINE->GetWindowInfo().height);
}

CameraComponent::CameraComponent(UPDATE_GROUP updateGroup) :
	Component(COMPONENT_TYPE::CAMERA, updateGroup),
	_matView(Matrix::Identity),
	_matProjection(Matrix::Identity),
	_matViewProjection(Matrix::Identity)
{
	SetUpdateEnabled(true);

	_nearZ = 1.f;
	_farZ = 5000.f;
	_fov = XM_PI / 4.f;
	_width = static_cast<float>(ENGINE->GetWindowInfo().width);
	_height = static_cast<float>(ENGINE->GetWindowInfo().height);
}

void CameraComponent::Render()
{
	shared_ptr<Scene> scene = GetOwnerGameObject()->GetScene();
	assert(scene != nullptr);
	
	if (shared_ptr<Skybox> skybox = scene->GetSkyBox())
		skybox->RenderSkybox();

	for (GameObject* gameObject : _forwardRenderObjects)
	{
		shared_ptr<MeshRenderer> meshRenderer = gameObject->GetMeshRenderer();
		assert(meshRenderer);		
		meshRenderer->Render();
	}
}

void CameraComponent::SortGameObjects()
{
	// TODO: Divide gameobjects by their render type. (forward, deferred, etc)
	_forwardRenderObjects.clear();

	assert(GetOwnerGameObject() != nullptr);
	const vector<shared_ptr<GameObject>>& gameObjects = GetOwnerGameObject()->GetScene()->GetGameObjects();
	for (const shared_ptr<GameObject>& gameObject : gameObjects)
	{
		if (gameObject->GetFixedComponent(COMPONENT_TYPE::MESH_RENDERER) != nullptr)
		{
			_forwardRenderObjects.push_back(gameObject.get());
		}
	}
}

void CameraComponent::FillCameraData(PerPassConstants& outPassConstants) const
{
	outPassConstants.cameraProjectionType = GetProjectionType();
}

const Matrix& CameraComponent::GetViewInverseMatrix() const
{
	assert(GetOwnerGameObject() != nullptr);
	return GetOwnerGameObject()->GetTransformComponent()->GetWorldMatrix();
}

void CameraComponent::Update()
{
	assert(GetOwnerGameObject() != nullptr);
	_matView = GetOwnerGameObject()->GetTransformComponent()->GetWorldMatrix().Invert();
	
	// TODO: Find a way not to do it on every update.
	if (GetProjectionType() == PROJECTION_TYPE::PERSPECTIVE)
	{
		const float ratio = GetWidth() / GetHeight();
		_matProjection = XMMatrixPerspectiveFovLH(GetFOV(), ratio, GetNearZ(), GetFarZ());
	}
	else // PROJECTION_TYPE::ORTHOGRAPHIC
	{
		_matProjection = XMMatrixOrthographicLH(GetWidth(), GetHeight(), GetNearZ(), GetFarZ());
	}

	_matViewProjection = _matView * _matProjection;
}
