#pragma once

#include "GameObject.h"
#include "LightComponent.h"

// TODO: Remove light on scene when destroyed.
class Light : public GameObject
{
public:
	// WARNING: Do not construct light directly. Use SpawnLight factory method.
	Light(Scene* scene, const LightInfo& info);
	virtual ~Light() = default;

	static shared_ptr<Light> SpawnLight(Scene* scene, const LightInfo& info);

public:
	inline const LightInfo& GetLightInfo() const { return GetLightComponent()->GetLightInfo(); }
	
	inline const Vec3& GetLightColor() const { return GetLightComponent()->GetLightColor(); }
	inline LIGHT_TYPE GetLightType() const { return GetLightComponent()->GetLightType(); }
	inline const Vec3& GetLightPosition() const { return GetLightComponent()->GetLightPosition(); }
	inline float GetLightRange() const { return GetLightComponent()->GetLightRange(); }
	inline const Vec3 GetLightDirection() const { return GetLightComponent()->GetLightDirection(); }
	inline float GetLightAngle() const { return GetLightComponent()->GetLightAngle(); }
	inline float GetLightFallOffStart() const { return GetLightComponent()->GetLightFallOffStart(); }
	inline float GetLightFallOffEnd() const { return GetLightComponent()->GetLightFallOffEnd(); }
	
	inline void SetLightInfo(const LightInfo& newInfo) { GetLightComponent()->SetLightInfo(newInfo); }
	
	inline void SetLightColor(const Vec3& newColor) { GetLightComponent()->SetLightColor(newColor); }
	inline void SetLightType(LIGHT_TYPE newType) { GetLightComponent()->SetLightType(newType); }
	inline void SetLightDirection(const Vec3& newDirection) { SetLightDirection(Quaternion::CreateFromYawPitchRoll(newDirection.z, newDirection.y, newDirection.x)); }
	inline void SetLightDirection(const Quaternion& newDirection) { GetTransformComponent()->SetLocalRotation(newDirection); }
	inline void SetLightRange(float newRange) { GetLightComponent()->SetLightRange(newRange); }
	inline void SetLightAngle(float newAngle) { GetLightComponent()->SetLightAngle(newAngle); }
	inline void SetLightFallOffStart(float newFallOffStartRatio) { GetLightComponent()->SetLightFallOffStart(newFallOffStartRatio); }
	inline void SetLightFallOffEnd(float newFallOffEndRatio) { GetLightComponent()->SetLightFallOffEnd(newFallOffEndRatio); }

public:
	int32 GetLightIdx() const { return _lightIdx; }
	inline void SetLightIdx(int32 newIdx) { _lightIdx = newIdx; }

private:
	int32 _lightIdx;
};

