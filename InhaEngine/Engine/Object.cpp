#include "pch.h"
#include "Object.h"

Object::Object(OBJECT_TYPE objectType, wstring name) : _objectType(objectType), _name(name)
{
	static uint32 idCount = 1;
	_id = idCount++;
}

Object::~Object()
{
}
