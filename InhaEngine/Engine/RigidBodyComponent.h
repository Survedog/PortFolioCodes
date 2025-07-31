#pragma once
#include "Component.h"
class RigidBodyComponent : public Component
{
public:
	RigidBodyComponent();

protected:
	virtual void OnOwnerGameObjectSet(GameObject* newOwnerGameObject) override;

public:
	// States
	inline Vec3 GetLinearVelocity() const { return _linearVelocity; }
	inline void SetLinearVelocity(const Vec3& velocity) { _linearVelocity = velocity; }

	inline Vec3 GetAngularVelocity() const { return _angularVelocity; }
	inline void SetAngularVelocity(const Vec3& velocity) { _angularVelocity = velocity; }

	inline Vec3 GetLinearAcceleration() const { return _linearAcceleration; }
	inline void SetLinearAcceleration(const Vec3& acceleration) { _linearAcceleration = acceleration; }

	inline Vec3 GetAngularAcceleration() const { return _angularAcceleration; }
	inline void SetAngularAcceleration(const Vec3& acceleration) { _angularAcceleration = acceleration; }

	// Attributes
	inline float GetMass() const { return _mass; }
	inline void SetMass(float mass) { _mass = mass; }

	inline float GetDensity() const { return _density; }
	inline void SetDensity(float density) { _density = density; }

	inline bool ShouldCalculateMassByDensity() const { return _calculateMassByDensity; }
	inline void SetCalculateMassByDensity(bool doCalculation) { _calculateMassByDensity = doCalculation; }

	inline float GetMaxLinearVelocity() const { return _maxLinearVelocity; }
	inline void SetMaxLinearVelocity(float maxVelocity) { _maxLinearVelocity = maxVelocity; }

	inline float GetMaxAngularVelocity() const { return _maxAngularVelocity; }
	inline void SetMaxAngularVelocity(float maxVelocity) { _maxAngularVelocity = maxVelocity; }

	inline float GetLinearDamping() const { return _linearDamping; }
	inline void SetLinearDamping(float damping) { _linearDamping = damping; }

	inline float GetAngularDamping() const { return _angularDamping; }
	inline void SetAngularDamping(float damping) { _angularDamping = damping; }

	inline float GetSleepThreshold() const { return _sleepThreshold; }
	inline void SetSleepThreshold(float threshold) { _sleepThreshold = threshold; }

	inline float GetStabilizationThreshold() const { return _stabilizationThreshold; }
	inline void SetStabilizationThreshold(float threshold) { _stabilizationThreshold = threshold; }

	inline uint32 GetPositionSolverIters() const { return _positionSolverIters; }
	inline void SetPositionSolverIters(uint32 iters) { _positionSolverIters = iters; }
	
	inline uint32 GetVelocitySolverIters() const { return _velocitySolverIters; }
	inline void SetVelocitySolverIters(uint32 iters) { _velocitySolverIters = iters; }

	inline bool IsGravityEnabled() const { return _gravityEnabled; }
	void SetGravityEnabled(bool enableGravity);

	inline bool IsSimulated() const { return _isSimulated; }
	inline void SetSimulated(bool simulate) { _isSimulated = simulate; }


private:
	// States
	Vec3 _linearVelocity = Vec3::Zero;
	Vec3 _angularVelocity = Vec3::Zero;

	Vec3 _linearAcceleration = Vec3::Zero;
	Vec3 _angularAcceleration = Vec3::Zero;

	// Attributes
	bool _calculateMassByDensity = true;
	float _mass = 640.f;
	float _density = 0.1f;

	float _maxLinearVelocity = 1000000.f;
	float _maxAngularVelocity = 1000000.f;

	float _linearDamping = 1.f;
	float _angularDamping = 1.f;

	float _sleepThreshold = 0.005f;
	float _stabilizationThreshold = 0.002f;
	uint32 _positionSolverIters = 4;
	uint32 _velocitySolverIters = 1;
	
	bool _gravityEnabled = true;
	bool _isSimulated = true; // TODO: Implement it.
};

