#include "pch.h"
#include "InputMappingContext.h"

bool InputMappingContext::CallInputListener(uint32 inputAction, float inputValue, INPUT_TRIGGER inputTrigger) const
{
	uint8 boundInput = _actionInputMapping[inputAction];
	uint8 trigger = static_cast<uint8>(inputTrigger);

	if (_inputListeners[boundInput][trigger] != nullptr)
	{
		float multipliedInputValue = inputValue * _actionInputValueMultiplier[inputAction];
		_inputListeners[boundInput][trigger](multipliedInputValue);
		return true;
	}

	return false;
}
