#pragma once
#include "Object.h"

enum class INPUT_TYPE : uint8
{
	KEYBOARD_MOUSE,
	CONTROLLER
};

enum class KEY_ACTION : uint32
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	LALT = VK_LMENU,
	RALT = VK_RMENU,
	LCTRL = VK_LCONTROL,
	RCTRL = VK_RCONTROL,
	LSHIFT = VK_LSHIFT,
	RSHIFT = VK_RSHIFT,

	ESC = VK_ESCAPE,
	TAB = VK_TAB,
	ENTER = VK_RETURN,
	BACKSPACE = VK_BACK,
	SPACE = VK_SPACE,

	SEMICOLON = VK_OEM_1,
	SLASH = VK_OEM_2,
	APOSTROPHE = VK_OEM_3,
	LBRACKET = VK_OEM_4,
	RBRACKET = VK_OEM_6,
	QUOTATION = VK_OEM_7,

	INSERT = VK_INSERT,
	HOME = VK_HOME,
	DEL = VK_DELETE,
	END = VK_END,
	PAGEUP = VK_PRIOR,
	PAGEDOWN = VK_NEXT,

	A = 'A', B = 'B', C = 'C', D = 'D',
	E = 'E', F = 'F', G = 'G', H = 'H',
	I = 'I', J = 'J', K = 'K', L = 'L',
	M = 'M', N = 'N', O = 'O', P = 'P',
	Q = 'Q', R = 'R', S = 'S', T = 'T',
	U = 'U', V = 'V', W = 'W', X = 'X',
	Y = 'Y', Z = 'Z',

	NUM0 = '0',
	NUM1 = '1',
	NUM2 = '2',
	NUM3 = '3',
	NUM4 = '4',
	NUM5 = '5',
	NUM6 = '6',
	NUM7 = '7',
	NUM8 = '8',
	NUM9 = '9',

	NUMPAD0 = VK_NUMPAD0,
	NUMPAD1 = VK_NUMPAD1,
	NUMPAD2 = VK_NUMPAD2,
	NUMPAD3 = VK_NUMPAD3,
	NUMPAD4 = VK_NUMPAD4,
	NUMPAD5 = VK_NUMPAD5,
	NUMPAD6 = VK_NUMPAD6,
	NUMPAD7 = VK_NUMPAD7,
	NUMPAD8 = VK_NUMPAD8,
	NUMPAD9 = VK_NUMPAD9,

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F4 = VK_F4,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12,

	LClick = VK_LBUTTON,
	RClick = VK_RBUTTON,
	MClick = VK_MBUTTON,

	ENUM_END = UINT8_MAX
};

enum class MOUSE_ACTION : uint32
{
	X = UINT8_MAX + 1,
	Y,
	XY, 
	WHEEL_UP,
	WHEEL_DOWN,

	ENUM_END // Warning: Not an actually valid item!
};

enum class INPUT_TRIGGER : uint8
{
	TRIGGERED,
	STARTED,
	ONGOING,
	COMPLETED,
	ENUM_END // Warning: Not an actually valid item!
};

enum
{
	KEY_ACTION_COUNT = static_cast<uint32>(KEY_ACTION::ENUM_END) + 1,
	MOUSE_ACTION_COUNT = static_cast<uint32>(MOUSE_ACTION::ENUM_END) - KEY_ACTION_COUNT,

	MAX_INPUT_COUNT = UINT8_MAX + 1,
	INPUT_TRIGGER_COUNT = static_cast<uint32>(INPUT_TRIGGER::ENUM_END),
};

// TODO: Need to be able to handle multiple listeners.
class InputMappingContext : Object
{
public:
	InputMappingContext(int32 priority) : InputMappingContext(KEY_ACTION_COUNT + MOUSE_ACTION_COUNT, priority)
	{
	}

	InputMappingContext(uint32 actionCount, int32 priority) :
		Object(OBJECT_TYPE::INPUT_MAPPING_CONTEXT, L"InputMappingContext"),
		_inputListeners(MAX_INPUT_COUNT, vector<function<void(float)>>(INPUT_TRIGGER_COUNT, nullptr)),
		_actionInputMapping(actionCount, NULL),
		_actionInputValueMultiplier(actionCount, NULL),
		_priority(priority)
	{
	}

public:
	// Returns true if there was valid listener for the input. (Returns false if not)
	bool CallInputListener(uint32 inputAction, float inputValue, INPUT_TRIGGER inputTrigger) const;

	// WARNING: The listener must be removed when the object gets removed!!
	template<typename T>
	void SetInputListener(uint8 input, T* object, void (T::* func)(float), INPUT_TRIGGER inputTrigger)
	{
		uint8 trigger = static_cast<uint8>(inputTrigger);
		_inputListeners[input][trigger] = [object, func](float inputValue)
			{
				(object->*func)(inputValue);
			};
	}

	inline void RemoveInputListener(uint8 input, INPUT_TRIGGER inputTrigger)
	{
		uint8 trigger = static_cast<uint8>(inputTrigger);
		_inputListeners[input][trigger] = nullptr;
	}

public:
	inline void BindInputKeyAction(KEY_ACTION keyAction, uint8 input, float inputValueMultiplier = 1.f) { BindInputAction(static_cast<uint32>(keyAction), input, inputValueMultiplier); }
	inline void BindInputMouseAction(MOUSE_ACTION mouseAction, uint8 input, float inputValueMultiplier = 1.f) { BindInputAction(static_cast<uint32>(mouseAction), input, inputValueMultiplier); }

	inline void UnbindInputAction(uint32 action)
	{
		_actionInputMapping[action] = NULL;
	}

private:
	// WARNING: The bound input must be grater than zero, and same or smaller than MAX_INPUT_COUNT.
	inline void BindInputAction(uint32 action, uint8 input, float inputValueMultiplier = 1.f)
	{
		assert(input != NULL && input < MAX_INPUT_COUNT);
		_actionInputMapping[action] = input;
		_actionInputValueMultiplier[action] = inputValueMultiplier;
	}

private:	
	vector<uint8> _actionInputMapping;
	vector<float> _actionInputValueMultiplier;
	vector<vector<function<void(float)>>> _inputListeners; // The input 0 listener must be invalid.

public:
	inline int32 GetPriority() const {	return _priority; }
	inline void SetPriority(int32 newPriority) { _priority = newPriority; }

	inline bool operator<(const InputMappingContext& other) const
	{
		return _priority < other._priority;
	}

	inline bool operator>(const InputMappingContext& other) const
	{
		return _priority > other._priority;
	}

private:
	int32 _priority = 0;
};
