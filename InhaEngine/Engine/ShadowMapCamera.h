#pragma once

#include "Camera.h"
#include "ShadowMapCameraComponent.h"

class Light;

class ShadowMapCamera : public Camera
{
	// Shadow map camera must only be spawned by the owner scene.
public:

	// WARNING: Do not construct shadow map camera directly. Use SpawnShadowMapCamera method.
	ShadowMapCamera(Scene* scene);
	virtual ~ShadowMapCamera() = default;

	static shared_ptr<ShadowMapCamera> SpawnShadowMapCamera(Scene* scene);

public:
	inline shared_ptr<ShadowMapCameraComponent> GetShadowMapCameraComponent() { return const_pointer_cast<ShadowMapCameraComponent>(CONST_THIS->GetShadowMapCameraComponent()); }
	inline shared_ptr<const ShadowMapCameraComponent> GetShadowMapCameraComponent() const
	{
		assert(GetFixedComponent(COMPONENT_TYPE::CAMERA) != nullptr);
		return static_pointer_cast<const ShadowMapCameraComponent>(GetFixedComponent(COMPONENT_TYPE::CAMERA));
	}

	inline void RenderShadowMap() { GetShadowMapCameraComponent()->RenderShadowMap(); }

public:
	// Texture matrix transforms NDC space (-1, 1)^2 to texture space (0, 1)^2.
	inline Matrix GetTextureMatrix() const { return GetShadowMapCameraComponent()->GetTextureMatrix(); }

	// Shadow matrix transforms world space to shadow map's texture space.
	inline Matrix GetShadowMatrix() const {	return GetViewProjectionMatrix() * GetTextureMatrix(); }

	inline uint32 GetTargetShadowMapIdx() const { return GetShadowMapCameraComponent()->GetTargetShadowMapIdx(); }
	inline void SetTargetShadowMapIdx(uint32 newTargetIdx) { GetShadowMapCameraComponent()->SetTargetShadowMapIdx(newTargetIdx); }

	inline shared_ptr<Light> GetTargetLight() { return GetShadowMapCameraComponent()->GetTargetLight(); }
	inline shared_ptr<const Light> GetTargetLight() const { return GetShadowMapCameraComponent()->GetTargetLight(); }	
	inline void SetTargetLight(shared_ptr<Light> newTargetLight) { GetShadowMapCameraComponent()->SetTargetLight(newTargetLight); }

	inline Vec2 GetFrustumPartitionRange() const { return GetShadowMapCameraComponent()->GetFrustumPartitionRange(); }
	inline void SetFrustumPartitionRange(const Vec2& newRange) { GetShadowMapCameraComponent()->SetFrustumPartitionRange(newRange); }
};

