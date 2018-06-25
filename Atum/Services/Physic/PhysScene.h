
#pragma once

#include "PhysObject.h"
#include "PhysController.h"
#include "PhysHeightmap.h"
#include "Support/Buffer.h"

using namespace physx;

class PhysScene
{
	friend class Physics;
	PxScene* scene = nullptr;
	bool needFetch = false;
	PxControllerManager* manager = nullptr;

public:

	struct RaycastDesc
	{
		Vector origin;
		Vector dir;
		float  length;
		Vector hitPos;
		Vector hitNormal;
	};

	PhysObject*     CreateBox(Vector size, Matrix trans, bool isStatic);
	PhysController* CreateController(PhysControllerDesc& desc);
	PhysHeightmap* CreateHeightmap(PhysHeightmap::Desc& desc, const char* name);

	bool RayCast(RaycastDesc& desc);

	void SetVisualization(bool set);
	void DrawVisualization();

	void Simulate(float dt);
	void FetchResults();
	void Release();
};