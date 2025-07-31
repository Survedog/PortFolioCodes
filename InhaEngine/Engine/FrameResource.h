#pragma once

#include "ConstantBuffer.h"
#include "LightComponent.h"
#include "CameraComponent.h"

// TODO: Move these constants declarations to ConstantBuffer.h
// Per every frame
struct GlobalConstants
{
	// Camera
	Matrix viewMatrices[MAX_CAMERAS];
	Matrix viewInvMatrices[MAX_CAMERAS];
	Matrix projMatrices[MAX_CAMERAS];
	Matrix viewProjMatrices[MAX_CAMERAS];

	// Shadow
	Matrix shadowCameraViewProjMatrices[MAX_SHADOW_MAP_CAMERAS];
	Matrix shadowMatrices[MAX_SHADOW_MAP_CAMERAS]; // Transforms world space to shadow map texture space.
	
	Vec2 shadowCamera1_PartitionRange;
	Vec2 shadowCamera2_PartitionRange;
	Vec2 shadowCamera3_PartitionRange;
	// Time
	float totalTime = 0.0f;
	float deltaTime = 0.0f;

	// Light
	uint32 lightCount = 0;
	Vec3 ambientLightColor = Vec3(0.f);
	LightInfo lights[MAX_LIGHTS];

	// Terrain
	float terrainRowScale = 1.f;
	float terrainColumnScale = 1.f;	
	float terrainHeightScale = 1.f;
	float __padding0;
};

// Per rendering for each camera.
struct PerPassConstants
{
	// Camera
	uint32 currentCameraIdx;
	PROJECTION_TYPE cameraProjectionType;
	Vec2 __padding0;
};

// Per rendering for each game object.
struct PerGameObjectConstants
{
	Matrix worldMatrix = Matrix::Identity;
	Matrix worldInvTransposeMatrix = Matrix::Identity;
};

// Per each animation sequence.
struct PerAnimationConstants
{
	Matrix boneMatrices[MAX_SKELETON_BONES];
	Matrix boneInvTransposeMatrices[MAX_SKELETON_BONES];
};

class FrameResource
{
public:
	FrameResource(ID3D12Device* device, uint32 passCount);

public:
	inline ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return _cmdAlloc; }

	inline uint64 GetFenceValue() const { return _fenceValue; }
	inline void SetFenceValue(uint64 num) { _fenceValue = num; }
	inline void RaiseFenceValue(uint64 num) { SetFenceValue(_fenceValue + num); }

	inline shared_ptr<ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) { return _constantBuffers[static_cast<uint8>(type)]; }
	inline shared_ptr<const ConstantBuffer> GetConstantBuffer(CONSTANT_BUFFER_TYPE type) const { return _constantBuffers[static_cast<uint8>(type)]; }

	inline void ClearConstantBuffers()
	{
		for (shared_ptr<ConstantBuffer> constBuffer : _constantBuffers)
			constBuffer->Clear();
	}

private:
	void CreateConstantBuffer(CONSTANT_BUFFER_TYPE type, uint32 bufferSize, uint32 count);

	array<shared_ptr<ConstantBuffer>, CONSTANT_BUFFER_COUNT> _constantBuffers;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAlloc;

	UINT64 _fenceValue = 0;
};