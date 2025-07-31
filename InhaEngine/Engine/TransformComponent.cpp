#include "pch.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Engine.h"
#include "TableDescriptorHeapManager.h"
#include "Scene.h"
#include "Camera.h"

TransformComponent::TransformComponent() :
	Component(COMPONENT_TYPE::TRANSFORM /*, UPDATE_GROUP::PostPhysics*/ ), // TODO: Decide update timing.
	_matLocal(Matrix::Identity),
	_matWorld(Matrix::Identity)
{
	_isMatrixDirty = true;
	SetUpdateEnabled(false);
}

shared_ptr<const TransformComponent> TransformComponent::GetParentTransformComponent() const
{
	if (GetOwnerGameObject() == nullptr) return nullptr;

	shared_ptr<const GameObject> parentGameObject = GetOwnerGameObject()->GetParent();
	if (parentGameObject == nullptr) return nullptr;
	return parentGameObject->GetTransformComponent();
}

shared_ptr<TransformComponent> TransformComponent::GetParentTransformComponent()
{	
	return const_pointer_cast<TransformComponent>(
		static_cast<const TransformComponent*>(this)->GetParentTransformComponent()
	);
}

void TransformComponent::GetDescendantTransformComponents(vector<shared_ptr<const TransformComponent>>& outDescendantTransformComponents) const
{
	outDescendantTransformComponents.clear();
	if (GetOwnerGameObject() == nullptr) return;

	vector<shared_ptr<const GameObject>> descendantGameObjects;
	GetOwnerGameObject()->GetDescendants(descendantGameObjects);

	for (const shared_ptr<const GameObject>& childGameObject : descendantGameObjects)
		if (childGameObject->GetTransformComponent() != nullptr)
			outDescendantTransformComponents.push_back(childGameObject->GetTransformComponent());
}

void TransformComponent::SetWorldPosition(const Vec3& newWorldPosition)
{
	Vec3 newLocalPosition = newWorldPosition;
	
	shared_ptr<TransformComponent> parentTransformComponents = GetParentTransformComponent();
	if (parentTransformComponents != nullptr)
	{
		newLocalPosition = newLocalPosition * GetWorldMatrix().Invert();
	}

	SetLocalPosition(newLocalPosition);
}

void TransformComponent::PushData() const
{
	assert(GetOwnerGameObject() != nullptr);
	shared_ptr<const Scene> scene = GetOwnerGameObject()->GetScene();

	PerGameObjectConstants gameObjectConstants;
	gameObjectConstants.worldMatrix = GetWorldMatrix();
	gameObjectConstants.worldInvTransposeMatrix = InverseTranspose(GetWorldMatrix());

	ENGINE->GetGraphicsDescHeapManager()->StartBuildingTable(DESC_TABLE_TYPE::TRANSFORM);
	ENGINE->GetConstantBuffer(CONSTANT_BUFFER_TYPE::PER_GAMEOBJECT)->PushGraphicsConstants(&gameObjectConstants, sizeof(gameObjectConstants));
	ENGINE->GetGraphicsDescHeapManager()->CommitTable(static_cast<uint32>(ROOT_PARAM::TRANSFORM), TRANSFORM_TABLE_DESC_COUNT);
}

void TransformComponent::SetMatrixDirty() const
{
	_isMatrixDirty = true;

	// TODO: Check if it makes too much overhead.
	vector<shared_ptr<const TransformComponent>> descendants;
	GetDescendantTransformComponents(descendants);
	for (const shared_ptr<const TransformComponent>& descendant : descendants)
	{
		descendant->SetMatrixDirty();
	}
}

void TransformComponent::CalcTransformMatrix() const
{
	if (_isMatrixDirty)
	{
		const XMVECTORF32 scaleVec = { _localTransform.scale.y, _localTransform.scale.z, _localTransform.scale.x };
		const XMVECTORF32 translationVec = { _localTransform.position.y, _localTransform.position.z, _localTransform.position.x };
		const XMVECTOR rotationVec = _localTransform.rotation;

		XMMATRIX localMatrix = DirectX::XMMatrixTransformation(g_XMZero, Quaternion::Identity, scaleVec, g_XMZero, rotationVec, translationVec);
		XMStoreFloat4x4(&_matLocal, localMatrix);
		_matWorld = _matLocal;

		shared_ptr<const TransformComponent> parentTransformComponent = GetParentTransformComponent();
		if (parentTransformComponent != nullptr)
		{
			_matWorld *= parentTransformComponent->GetWorldMatrix();
		}

		_isMatrixDirty = false;
	}
}
