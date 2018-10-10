
#pragma once

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#include "PxPhysicsAPI.h"

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG
#endif

#include "Support/Support.h"

using namespace physx;

struct PhysControllerDesc
{
	float height = 1.0f;
	float radius = 0.25f;
	Vector pos;
	float slopeLimit = cosf(RADIAN * 20.0f);
};

class PhysController
{
	friend class PhysScene;
	PxController* controller = nullptr;
public:
	void Move(Vector dir);
	void SetPosition(Vector& pos);
	void GetPosition(Vector& pos);
	void Release();
};
