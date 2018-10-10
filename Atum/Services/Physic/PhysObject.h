
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

using namespace physx;

class PhysObject
{
	friend class PhysScene;
	bool isStatic = false;
	PxRigidActor* actor = nullptr;
public:

	bool IsSatic();
	void GetTransform(Matrix& mat);
	void AddForceAt(Vector pos, Vector force);
	void Release();
};
