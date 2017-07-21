#pragma once

class Object
{
public:
	typedef void (Object::*Delegate)(float timedelta);

	virtual ~Object() {};
	virtual void Release() {};
};

struct DelegateObject
{
	int level;
	Object* entity;
	Object::Delegate call;
};
