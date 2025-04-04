#include "pch.h"
#include "Object.h"

Object::Object(OBJECT_TYPE type) : _objectType(type)
{
	// static 공용으로 사용
	static uint32 idGenerator = 1;
	_id = idGenerator;
	idGenerator++;
}

Object::~Object()
{

}
