#include "pch.h"
#include "ShadowMapCameraComponent.h"
#include "Scene.h"
#include "Light.h"

ShadowMapCameraComponent::ShadowMapCameraComponent() :
	CameraComponent(UPDATE_GROUP::PostUpdateWork)
{
}

void ShadowMapCameraComponent::RenderShadowMap()
{
	vector<GameObject*> forwardRenderObjects;
	GetForwardRenderObjects(forwardRenderObjects);

	for (GameObject* gameObject : forwardRenderObjects)
	{
		shared_ptr<MeshRenderer> meshRenderer = gameObject->GetMeshRenderer();
		assert(meshRenderer);
		meshRenderer->RenderShadowMap();
	}
}

void ShadowMapCameraComponent::Update()
{
	// Fit the shadow rendering area in the main camera's view frustum.
	// NOTE: It assumes that the target light is directional light.
	if (GetTargetLight() != nullptr)
	{
		const float fitOffset = 5.f; // Offset of main camera frustum's AABB in all direction.
		const float shadowMapCamOffset = 10000.f; // Offset from main camera frustum's center to shadow map camera.

		shared_ptr<GameObject> ownerGameObject = GetOwnerGameObject();
		assert(ownerGameObject != nullptr && ownerGameObject->GetScene() != nullptr);

		shared_ptr<Camera> mainCamera = ownerGameObject->GetScene()->GetMainCamera();
		assert(mainCamera != nullptr);

		// Calculate View Matrix.
		const float frustumNZ = mainCamera->GetNearZ();
		const float frustumFZ = mainCamera->GetFarZ();
		
		const float partitionStartZ = (frustumFZ - frustumNZ) * GetFrustumPartitionRange().x + frustumNZ; // The start Z plane of frustum partition.
		const float partitionEndZ = (frustumFZ - frustumNZ) * GetFrustumPartitionRange().y + frustumNZ; // The end Z plane of frustum partition.
		const float partitionCenterZ = (partitionStartZ + partitionEndZ) / 2.f; // The center Z position of the frustum partition.
		
		const float ClipPartitionStartZ = frustumFZ * (partitionStartZ - frustumNZ) / (frustumFZ - frustumNZ); // Frustum partition start plane's Z position in homogeneous clip space.
		const float ClipPartitionEndZ = frustumFZ * (partitionEndZ - frustumNZ) / (frustumFZ - frustumNZ); // Frustum partition end plane's Z position in homogeneous clip space.

		Vec3 partitionCenterW = mainCamera->GetWorldPosition() + mainCamera->GetTransformComponent()->GetForward() * partitionCenterZ;
		Vec3 shadowMapCamPosW = partitionCenterW - GetTargetLight()->GetLightDirection() * shadowMapCamOffset;

		Matrix viewMatrix = XMMatrixLookAtLH(
			GetDxCoordsFromGameCoords(shadowMapCamPosW),
			GetDxCoordsFromGameCoords(partitionCenterW),
			GetDxCoordsFromGameCoords(mainCamera->GetTransformComponent()->GetUp())
		);
		SetViewMatrix(viewMatrix);

		// Calculate projection matrix.
		{
			// Frustum corner points' position in homogeneous clip space.
			vector<Vec4> frustumCornerPoints = {
				// Near Plane
				{ -partitionStartZ, partitionStartZ, ClipPartitionStartZ, partitionStartZ},
				{ partitionStartZ, partitionStartZ, ClipPartitionStartZ, partitionStartZ},
				{ partitionStartZ, -partitionStartZ, ClipPartitionStartZ, partitionStartZ},
				{ -partitionStartZ, -partitionStartZ, ClipPartitionStartZ, partitionStartZ},

				// Far Plane
				{ -partitionEndZ, partitionEndZ, ClipPartitionEndZ, partitionEndZ},
				{ partitionEndZ, partitionEndZ, ClipPartitionEndZ, partitionEndZ},
				{ partitionEndZ, -partitionEndZ, ClipPartitionEndZ, partitionEndZ},
				{ -partitionEndZ, -partitionEndZ, ClipPartitionEndZ, partitionEndZ}, // Far Plane
			};

			// Find AABB of main camera's frustum in shadow map camera's view space.
			float rightmost = -FLT_MAX, leftmost = FLT_MAX, uppermost = -FLT_MAX, lowermost = FLT_MAX, furthest = -FLT_MAX, nearest = FLT_MAX;
			Matrix clipToShadowCamView = mainCamera->GetViewProjectionMatrix().Invert() * viewMatrix; // Transform matrix that converts main camera's clip space to shadow map camera's view space.

			for (Vec4& point : frustumCornerPoints)
			{
				// Transform the positions from main camera's clip space to shadow map camera's view space.
				point = GetGameCoordsFromDXCoords(XMVector4Transform(point, clipToShadowCamView));
				rightmost = max(rightmost, point.y);
				leftmost = min(leftmost, point.y);
				uppermost = max(uppermost, point.z);
				lowermost = min(lowermost, point.z);
				furthest = max(furthest, point.x);
				nearest = min(nearest, point.x);
			}

			SetWidth(max(rightmost, -leftmost) * 2 + fitOffset);
			SetHeight(max(uppermost, -lowermost) * 2 + fitOffset);
			SetNearZ(100.f);
			SetFarZ(furthest + fitOffset);
		
			SetProjectionMatrix(XMMatrixOrthographicLH(GetWidth(), GetHeight(), GetNearZ(), GetFarZ()));
		}

		// TODO: Find a way not to do it on every update.
		SetViewProjectionMatrix(GetViewMatrix() * GetProjectionMatrix());
	}
}
