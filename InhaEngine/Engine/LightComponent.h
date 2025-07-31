#pragma once

#include "Component.h"

enum class LIGHT_TYPE : int32
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct LightInfo
{
	Vec3 color = { 1.f, 1.f, 1.f };
	LIGHT_TYPE lightType = LIGHT_TYPE::DIRECTIONAL_LIGHT;
	Vec3 position = { 0.f, 0.f, 0.f };
	float range = 0.f;
	Vec3 direction = { 1.f, 0.f, 0.f };
	float angle = 0.f;
	float fallOffStartRatio = 0.f;
	float fallOffEndRatio = 1.f;
	Vec2 __padding0;
};

class LightComponent : public Component
{
public:
	LightComponent(LightInfo info);
	virtual ~LightComponent() = default;

public:
	inline const LightInfo& GetLightInfo() const { return _lightInfo; }

	inline const Vec3& GetLightColor() const { return _lightInfo.color; }
	inline LIGHT_TYPE GetLightType() const { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); } // TODO: Remove static_cast if safe.
	inline const Vec3& GetLightPosition() const { return _lightInfo.position; }
	inline float GetLightRange() const { return _lightInfo.range; }
	inline const Vec3& GetLightDirection() const { return _lightInfo.direction; }
	inline float GetLightAngle() const { return _lightInfo.angle; }
	inline float GetLightFallOffStart() const { return _lightInfo.fallOffStartRatio; }
	inline float GetLightFallOffEnd() const { return _lightInfo.fallOffEndRatio; }

	inline void SetLightInfo(const LightInfo& newInfo) { _lightInfo = newInfo; }

	inline void SetLightColor(const Vec3& newColor) { _lightInfo.color = newColor; }
	inline void SetLightType(LIGHT_TYPE newLightType) { _lightInfo.lightType = newLightType; }
	inline void SetLightDirection(const Vec3& newDirection) { _lightInfo.direction = newDirection; }
	inline void SetLightRange(float newRange) { _lightInfo.range = newRange; }
	inline void SetLightAngle(float newAngle) { _lightInfo.angle = newAngle; }
	inline void SetLightFallOffStart(float newFallOffStartRatio) { _lightInfo.fallOffStartRatio = newFallOffStartRatio; }
	inline void SetLightFallOffEnd(float newFallOffEndRatio) { _lightInfo.fallOffEndRatio = newFallOffEndRatio; }

protected:
	inline void SetLightPosition(const Vec3& newPosition) { _lightInfo.position = { newPosition.x, newPosition.y, newPosition.z, 1.f}; }

	virtual void Update() override;

private:
	LightInfo _lightInfo;	
};

