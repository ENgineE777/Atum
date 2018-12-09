
#pragma once

#include "PhysObject.h"
#include "PhysController.h"
#include "PhysHeightmap.h"

using namespace physx;

class PhysScene : public PxSimulationEventCallback
{
	friend class Physics;
	PxScene* scene = nullptr;
	bool needFetch = false;
	PxControllerManager* manager = nullptr;

public:

	struct BodyUserData
	{
		class SceneObject* object = nullptr;
		int index = -1;
		PhysObject* body = nullptr;
		class PhysController* controller = nullptr;
	};

	struct RaycastDesc
	{
		Vector origin;
		Vector dir;
		float  length;
		Vector hitPos;
		Vector hitNormal;
		BodyUserData* userdata = nullptr;
	};

	PhysObject*     CreateBox(Vector size, Matrix trans, Matrix offset, PhysObject::BodyType type);
	PhysController* CreateController(PhysControllerDesc& desc);
	PhysHeightmap* CreateHeightmap(PhysHeightmap::Desc& desc, const char* name);

	bool RayCast(RaycastDesc& desc);

	void SetVisualization(bool set);
	void DrawVisualization();

	void Simulate(float dt);
	void FetchResults();
	void Release();

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {};
	void onWake(PxActor** actors, PxU32 count) {};
	void onSleep(PxActor** actors, PxU32 count) {};
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};
};