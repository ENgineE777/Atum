
#pragma once

#ifdef PLATFORM_IOS

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#endif

/**
\ingroup gr_code_services_physic
*/

#include "PxPhysicsAPI.h"

#ifdef PLATFORM_IOS

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG

#endif

#endif

#include "Support/Support.h"

using namespace physx;

struct PhysControllerDesc
{
	float    height = 1.0f;
	float    radius = 0.25f;
	Vector   pos;
	uint32_t group;
	float    slopeLimit = cosf(RADIAN * 20.0f);
};

class PhysController : public PxUserControllerHitReport, PxControllerBehaviorCallback, PxQueryFilterCallback
{
	friend class PhysScene;
	PxController* controller = nullptr;
	float height = 1.0f;
	bool active = true;
	Vector deactive_pos;

public:

	enum CollideType
	{
		CollideSides = (1 << 0),
		CollideUp = (1 << 1),
		CollideDown = (1 << 2)
	};

	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags);
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit);

	// Implements PxUserControllerHitReport
	virtual void							onShapeHit(const PxControllerShapeHit& hit);
	virtual void							onControllerHit(const PxControllersHit& hit) {}
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit) {}

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

	void SetActive(bool set);
	bool IsActive();
	void SetUserData(void* data);
	void* GetUserData();
	float GetHeight();

	bool IsColliding(CollideType type);
	void Move(Vector dir);
	void SetGroup(int group);
	void SetPosition(Vector pos);
	void GetPosition(Vector& pos);
	void Release();
};
