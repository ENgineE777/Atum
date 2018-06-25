
#pragma once

#include "PxPhysicsAPI.h"
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