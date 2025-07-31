#include "pch.h"
#include "InputComponent.h"
#include "Engine.h"
#include "GameObject.h"

InputComponent::InputComponent() :
	Component(COMPONENT_TYPE::INPUT, UPDATE_GROUP::PrePhysics) // TODO: It's update order should precede other components.
{
	_mousePos = _lastMousePos = { 0, 0 };

	SetUpdateEnabled(true);
}

void InputComponent::Initialize(GameObject* ownerGameObject)
{
	Component::Initialize(ownerGameObject);

	::GetCursorPos(&_mousePos);
	::ScreenToClient(ENGINE->GetWindowInfo().hwnd, &_mousePos);
	_lastMousePos = _mousePos;
	memset(&_actionTriggeredLastFrame[0], false, _actionTriggeredLastFrame.size());
}

void InputComponent::CallInputListener(uint32 inputAction, float inputValue, INPUT_TRIGGER inputTrigger)
{
	int32 back = static_cast<int32>(_inputMappingContexts.size()) - 1;
	for (int32 idx = back; idx >= 0; --idx)
	{
		if (_inputMappingContexts[idx].CallInputListener(inputAction, inputValue, inputTrigger))
			return;
	}
}

void InputComponent::Update()
{
	HWND activeHWND = ::GetActiveWindow();
	if (activeHWND != ENGINE->GetWindowInfo().hwnd)
	{
		memset(&_actionTriggeredLastFrame[0], false, KEY_ACTION_COUNT);
		return;
	}
	
	CheckKeyboardInput();
	CheckMouseInput();
}

void InputComponent::CheckKeyboardInput()
{
	for (uint32 i = 0; i < KEY_ACTION_COUNT; ++i)
	{
		KEY_ACTION keyAction = static_cast<KEY_ACTION>(i);

		if (::GetAsyncKeyState(i) & 0x8000) // Key is pressed (down)
		{
			const float inputValue = 1.0f; // TODO: Get real input value

			if (_actionTriggeredLastFrame[i] == false)
				CallInputListener(keyAction, inputValue, INPUT_TRIGGER::STARTED);
			else
				CallInputListener(keyAction, inputValue, INPUT_TRIGGER::ONGOING);

			CallInputListener(keyAction, inputValue, INPUT_TRIGGER::TRIGGERED);
			_actionTriggeredLastFrame[i] = true;
		}
		else // Key is not pressed
		{
			const float inputValue = 0.0f; // TODO: Get real input value

			if (_actionTriggeredLastFrame[i])
				CallInputListener(keyAction, inputValue, INPUT_TRIGGER::COMPLETED);

			_actionTriggeredLastFrame[i] = false;
		}
	}
}

void InputComponent::CheckMouseInput()
{
	const int mouseActionStart = static_cast<uint32>(MOUSE_ACTION::X);
	_lastMousePos = _mousePos;
	::GetCursorPos(&_mousePos);
	::ScreenToClient(ENGINE->GetWindowInfo().hwnd, &_mousePos);

	//TODO: Implement XY, WheelDown, WheelUp input checking.
	POINT mouseMovement(_mousePos.x - _lastMousePos.x, _mousePos.y - _lastMousePos.y);
	float inputValues[MOUSE_ACTION_COUNT] = {};

	inputValues[static_cast<uint32>(MOUSE_ACTION::X) - mouseActionStart] = static_cast<float>(_mousePos.x - _lastMousePos.x);
	inputValues[static_cast<uint32>(MOUSE_ACTION::Y) - mouseActionStart] = static_cast<float>(_mousePos.y - _lastMousePos.y);

	for (uint32 action = mouseActionStart; action < static_cast<uint32>(MOUSE_ACTION::ENUM_END); ++action)
	{
		float inputValue = inputValues[action - mouseActionStart];
		if (inputValue == 0.f) // input action not 
		{
			if (_actionTriggeredLastFrame[action])
				CallInputListener(action, inputValue, INPUT_TRIGGER::COMPLETED);
			_actionTriggeredLastFrame[action] = false;
		}
		else
		{
			if (!_actionTriggeredLastFrame[action])
				CallInputListener(action, inputValue, INPUT_TRIGGER::STARTED);
			else
				CallInputListener(action, inputValue, INPUT_TRIGGER::ONGOING);

			CallInputListener(action, inputValue, INPUT_TRIGGER::TRIGGERED);
			_actionTriggeredLastFrame[action] = true;
		}
	}
	
}
