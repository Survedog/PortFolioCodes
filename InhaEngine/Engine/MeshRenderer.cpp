#include "pch.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "AnimationInstance.h"

MeshRenderer::MeshRenderer() :
	Component(COMPONENT_TYPE::MESH_RENDERER, UPDATE_GROUP::PostPhysics)
{
	SetUpdateEnabled(true);
}

void MeshRenderer::Render()
{
	if (_mesh == nullptr) return; // TODO: Make error message.

	assert(GetOwnerGameObject() != nullptr);
	_mesh->Render(GetOwnerGameObject()->GetTransformComponent());
}

void MeshRenderer::RenderShadowMap()
{
	if (_mesh == nullptr) return; // TODO: Make error message.

	assert(GetOwnerGameObject() != nullptr);
	_mesh->RenderShadowMap(GetOwnerGameObject()->GetTransformComponent());
}

void MeshRenderer::Start()
{
	shared_ptr<SkeletalMesh> skeletalMesh = GetSkeletalMesh();
	if (skeletalMesh != nullptr)
	{
		shared_ptr<AnimationInstance> animInstance = skeletalMesh->GetAnimationInstance();
		if (animInstance != nullptr)
		{
			animInstance->OnAnimationInit();
		}
	}
}

void MeshRenderer::Update()
{
	shared_ptr<SkeletalMesh> skeletalMesh = GetSkeletalMesh();
	if (skeletalMesh != nullptr)
	{
		shared_ptr<AnimationInstance> animInstance = skeletalMesh->GetAnimationInstance();
		if (animInstance != nullptr)
		{
			animInstance->OnAnimationTick();
		}
	}
}

void MeshRenderer::LoadMesh(const wstring& path)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Load(path);
	SetMesh(mesh);
}
