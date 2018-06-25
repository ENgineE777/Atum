
#pragma once

#include "PxPhysicsAPI.h"
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