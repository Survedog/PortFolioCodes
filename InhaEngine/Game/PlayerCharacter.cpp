#include "pch.h"
#include "PlayerCharacter.h"
#include "Engine.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "Scene.h"
#include "DirectionalLight.h"
#include "Timer.h"

PlayerCharacter::PlayerCharacter(Scene* scene) :
	GameObject(scene)
{
	AddFixedComponent(new CameraComponent());
	AddFixedComponent(new InputComponent());
}

void PlayerCharacter::InitializeComponents()
{
	GameObject::InitializeComponents();

	const int moveForwardInput = 1;
	const int moveRightInput = 2;
	const int moveUpInput = 3;
	
	const int lookHorizontalInput = 4;
	const int lookVerticalInput = 5;

	const int lookHorizontalMouseInput = 6;
	const int lookVerticalMouseInput = 7;
	const int lookAroundInput = 8;

	const int rotateLightYawInput = 9;
	const int rotateLightPitchInput = 10;

	InputMappingContext imc(0);
	imc.BindInputKeyAction(KEY_ACTION::W, moveForwardInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::S, moveForwardInput, -1.f);
	imc.SetInputListener(moveForwardInput, this, &PlayerCharacter::moveForward, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::D, moveRightInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::A, moveRightInput, -1.f);
	imc.SetInputListener(moveRightInput, this, &PlayerCharacter::moveRight, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::SPACE, moveUpInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::LCTRL, moveUpInput, -1.f);
	imc.SetInputListener(moveUpInput, this, &PlayerCharacter::moveUp, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::LClick, lookAroundInput);
	imc.SetInputListener(lookAroundInput, this, &PlayerCharacter::StartLookingAround, INPUT_TRIGGER::STARTED);
	imc.SetInputListener(lookAroundInput, this, &PlayerCharacter::EndLookingAround, INPUT_TRIGGER::COMPLETED);

	imc.BindInputKeyAction(KEY_ACTION::NUMPAD6, lookHorizontalInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::NUMPAD4, lookHorizontalInput, -1.f);
	imc.SetInputListener(lookHorizontalInput, this, &PlayerCharacter::LookYaw, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::NUMPAD8, lookVerticalInput, -1.f);
	imc.BindInputKeyAction(KEY_ACTION::NUMPAD5, lookVerticalInput, 1.f);
	imc.SetInputListener(lookVerticalInput, this, &PlayerCharacter::LookPitch, INPUT_TRIGGER::TRIGGERED);
	
	imc.BindInputMouseAction(MOUSE_ACTION::X, lookHorizontalMouseInput);
	imc.SetInputListener(lookHorizontalMouseInput, this, &PlayerCharacter::LookYawWithMouse, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputMouseAction(MOUSE_ACTION::Y, lookVerticalMouseInput);
	imc.SetInputListener(lookVerticalMouseInput, this, &PlayerCharacter::LookPitchWithMouse, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::LBRACKET, rotateLightYawInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::RBRACKET, rotateLightYawInput, -1.f);
	imc.SetInputListener(rotateLightYawInput, this, &PlayerCharacter::RotateMainLightYaw, INPUT_TRIGGER::TRIGGERED);

	imc.BindInputKeyAction(KEY_ACTION::SEMICOLON, rotateLightPitchInput, 1.f);
	imc.BindInputKeyAction(KEY_ACTION::QUOTATION, rotateLightPitchInput, -1.f);
	imc.SetInputListener(rotateLightPitchInput, this, &PlayerCharacter::RotateMainLightPitch, INPUT_TRIGGER::TRIGGERED);

	GetInputComponent()->AddInputMappingContext(move(imc));
}

void PlayerCharacter::moveForward(float inputValue)
{
	Vec3 newPosition = GetWorldPosition();
	newPosition += GetTransformComponent()->GetForward() * inputValue * GetMoveSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	SetWorldPosition(newPosition);
}

void PlayerCharacter::moveRight(float inputValue)
{
	Vec3 newPosition = GetWorldPosition();
	newPosition += GetTransformComponent()->GetRight() * inputValue * GetMoveSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	SetWorldPosition(newPosition);
}

void PlayerCharacter::moveUp(float inputValue)
{
	Vec3 newPosition = GetWorldPosition();
	newPosition += Vec3(0.f, 0.f, 1.f) * inputValue * GetMoveSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	SetWorldPosition(newPosition);
}

void PlayerCharacter::StartLookingAround(float inputValue)
{
	_isLookingAround = true;
}

void PlayerCharacter::EndLookingAround(float inputValue)
{
	_isLookingAround = false;
}

void PlayerCharacter::LookYaw(float inputValue)
{
	float angle = inputValue * GetLookSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	Vec3 worldUpAxis = Vec3(0.f, 0.f, 1.f);
	GetTransformComponent()->RotateAround(worldUpAxis, angle);
}
 
void PlayerCharacter::LookYawWithMouse(float inputValue)
{
	if (_isLookingAround)
	{
		LookYaw(inputValue * 1.5f);
	}
}

void PlayerCharacter::LookPitch(float inputValue)
{
	float angle = inputValue * GetLookSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	Vec3 rightAxis = GetTransformComponent()->GetRight();
	GetTransformComponent()->RotateAround(rightAxis, angle);
}

void PlayerCharacter::LookPitchWithMouse(float inputValue)
{
	if (_isLookingAround)
	{
		LookPitch(inputValue * 1.5f);
	}
}

void PlayerCharacter::RotateMainLightYaw(float inputValue)
{
	_lightYaw = _lightYaw + inputValue * GetLightRotateSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	_lightYaw = fmod(_lightYaw + 180.f, 360.f) - 180.f;
	RotateMainLight(_lightYaw, _lightPitch);
}

void PlayerCharacter::RotateMainLightPitch(float inputValue)
{
	_lightPitch = _lightPitch + inputValue * GetLightRotateSpeed() * ENGINE->GetGameTimer()->GetDeltaTime();
	_lightPitch = clamp(_lightPitch, -90.f, 90.f);
	RotateMainLight(_lightYaw, _lightPitch);
}

void PlayerCharacter::RotateMainLight(float newYaw, float newPitch) const
{
	shared_ptr<DirectionalLight> mainLight = ENGINE->GetActiveScene()->GetMainLight();
	if (mainLight != nullptr)
	{
		newYaw = XMConvertToRadians(newYaw);
		newPitch = XMConvertToRadians(newPitch);
		mainLight->SetLightDirection(Quaternion::CreateFromYawPitchRoll(newYaw, newPitch, 0.f));
	}
}
