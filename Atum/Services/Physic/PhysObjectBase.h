
#pragma once

#ifdef PLATFORM_IOS

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#endif

#include "PxPhysicsAPI.h"

#ifdef PLATFORM_IOS

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG

#endif

#endif

#include "Support/Support.h"

/**
\ingroup gr_code_services_physic
*/

/**
\brief PhysObject

Just a physical box.

*/

using namespace physx;

class PhysObjectBase
{
	friend class Physics;

public:

	/**
	\brief PhysObjectBase should released only via this mehod
	*/

	virtual void Release();

protected:
	virtual void ActualRelease() = 0;
};
