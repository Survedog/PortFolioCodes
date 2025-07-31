#pragma once

#include "CameraComponent.h"

class Light;

class ShadowMapCameraComponent : public CameraComponent
{
public:
	ShadowMapCameraComponent();
	virtual ~ShadowMapCameraComponent() = default;

public:
	void RenderShadowMap();

	inline uint32 GetTargetShadowMapIdx() const { return _targetShadowMapIdx; }
	inline void SetTargetShadowMapIdx(uint32 newTargetIdx) { _targetShadowMapIdx = newTargetIdx; }

	inline shared_ptr<Light> GetTargetLight() { return _targetLight; }
	inline shared_ptr<const Light> GetTargetLight() const { return _targetLight; }
	inline void SetTargetLight(shared_ptr<Light> newTargetLight) { _targetLight = newTargetLight; }

	inline Vec2 GetFrustumPartitionRange() const { return _frustumPartitionRange; }
	inline void SetFrustumPartitionRange(const Vec2& newRange)
	{
		assert(newRange.x >= 0.f && newRange.y <= 1.f && newRange.x < newRange.y);
		_frustumPartitionRange = newRange;
	}

	// Texture matrix transforms NDC space (-1, 1)^2 to texture space (0, 1)^2.
	inline Matrix GetTextureMatrix() const
	{
		return Matrix(0.5f, 0.f, 0.f, 0.f,
					  0.f, -0.5f, 0.f, 0.f,
					  0.f, 0.f, 1.f, 0.f,
					  0.5f, 0.5f, 0.f, 1.f);
	}

protected:
	virtual void Update() override;

private:
	// Index of the shadow map texture in SHADOW render target group to draw shadow map on.
	uint32 _targetShadowMapIdx = 0;
	shared_ptr<Light> _targetLight = nullptr;

	// Range of frustum that this shadow map will draw shadow.
	// First element is the start of the partition, and second element is the end.
	// The second element must be greater than the first element.
	// 0.f means the near plane of the main camera's frustum, and 1.f means the far plane of the main camera's frustum.
	Vec2 _frustumPartitionRange = Vec2(0.f, 1.f);
};

