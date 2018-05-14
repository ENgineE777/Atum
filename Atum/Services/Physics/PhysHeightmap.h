
#pragma once

#include "PxPhysicsAPI.h"
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