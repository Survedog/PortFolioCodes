#pragma once

#include "GameObject.h"

class PlayerCharacter : public GameObject
{
public:
	PlayerCharacter(Scene* scene);
	virtual ~PlayerCharacter() = default;

public:
	virtual void InitializeComponents() override;

public:
	void moveForward(float inputValue);
	void moveRight(float inputValue);
	void moveUp(float inputValue);

	void StartLookingAround(float inputValue);
	void EndLookingAround(float inputValue);

	void LookYaw(float inputValue);
	void LookYawWithMouse(float inputValue);
	void LookPitch(float inputValue);
	void LookPitchWithMouse(float inputValue);

public:
	inline float GetMoveSpeed() const { return _moveSpeed; }
	inline float GetLookSpeed() const { return _lookSpeed; }

private:
	float _moveSpeed = 100.f;
	float _lookSpeed = 150.f;
	bool _isLookingAround = false;

// Light Movement
public:
	void RotateMainLightYaw(float inputValue);
	void RotateMainLightPitch(float inputValue);
	inline float GetLightRotateSpeed() const { return _lightRotateSpeed; }

private:
	void RotateMainLight(float newYaw, float newPitch) const;

private:
	float _lightRotateSpeed = 45.f;
	float _lightYaw = 0.f;
	float _lightPitch = 45.f;
};

