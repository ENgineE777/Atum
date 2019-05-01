
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

using namespace physx;

class PhysObject
{
	friend class PhysScene;

public:

	enum BodyType
	{
		Static = 0,
		Dynamic,
		DynamicCCD,
		Kinetic,
		Trigger
	};

	void SetActive(bool set);
	bool IsActive();
	void SetUserData(void* data);
	void* GetUserData();
	void SetFixedRotation(bool set);
	BodyType GetType();
	void SetGroup(int group);
	void SetTransform(Matrix& mat);
	void GetTransform(Matrix& mat);
	void AddForceAt(Vector pos, Vector force);
	void Release();

protected:
	BodyType body_type;
	PxRigidActor* actor = nullptr;
	bool is_active = true;
};
