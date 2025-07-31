#pragma once

#include "Component.h"
#include "InputMappingContext.h"

// TODO: Make it an abstract class.
class InputComponent : public Component
{
public:
	InputComponent();
	virtual ~InputComponent() = default;

public:
	virtual void Initialize(GameObject* ownerGameObject) override;

	// TODO: Check how much copying overhead is ocurred.
	inline void AddInputMappingContext(InputMappingContext&& inputMappingContext)
	{
		_inputMappingContexts.push_back(inputMappingContext);
		sort(_inputMappingContexts.begin(), _inputMappingContexts.end());
	}

	inline void RemoveInputMappingContext(uint32 idx) {	_inputMappingContexts.erase(_inputMappingContexts.begin() + idx); }
	inline void ClearAllInputMappingContexts() { _inputMappingContexts.clear(); }

public:
	inline POINT GetMousePos() { return _mousePos; }

private:
	virtual void Update() override;

	void CheckKeyboardInput();
	void CheckMouseInput();

	inline void CallInputListener(KEY_ACTION keyAction, float inputValue, INPUT_TRIGGER inputTrigger) { CallInputListener(static_cast<uint32>(keyAction), inputValue, inputTrigger); }
	inline void CallInputListener(MOUSE_ACTION mouseAction, float inputValue, INPUT_TRIGGER inputTrigger) { CallInputListener(static_cast<uint32>(mouseAction), inputValue, inputTrigger); }
	void CallInputListener(uint32 inputAction, float inputValue, INPUT_TRIGGER inputTrigger);

private:
	vector<InputMappingContext> _inputMappingContexts;
	
	// True if matching input action was triggered last frame.
	array<bool, KEY_ACTION_COUNT + MOUSE_ACTION_COUNT> _actionTriggeredLastFrame = {};
	POINT _mousePos;
	POINT _lastMousePos;
};

