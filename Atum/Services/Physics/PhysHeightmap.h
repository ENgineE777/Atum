
#pragma once

#include "PxPhysicsAPI.h"
#include "Support/Support.h"

using namespace physx;

struct PhysHeightmapDesc
{
	int      width;
	int      height;
	Vector2  scale;
	uint8_t* hmap;
};

class PhysHeightmap
{
	friend class PhysScene;
	PxHeightFieldSample* samples;
	PxHeightField*       heightField;
	PxRigidActor*        actor;
public:
	void Release();
};