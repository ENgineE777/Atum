#pragma once

/**
\ingroup gr_code_common
*/

class Object
{
public:
	typedef void (Object::*Delegate)(float timedelta);

	virtual ~Object() {};
	virtual void Release() {};
};

struct DelegateObject
{
	Object* entity;
	Object::Delegate call;
};
