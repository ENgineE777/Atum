
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


class PhysHeightmap
{
	friend class PhysScene;
	PxHeightField* heightField = nullptr;
	PxRigidActor*  actor = nullptr;
public:

	struct Desc
	{
		int      width;
		int      height;
		Vector2  scale;
#ifdef PLATFORM_PC
		uint8_t* hmap = nullptr;
#endif
	};

	void Release();
};
