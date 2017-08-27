
#pragma once

#include "PhysScene.h"
#include <vector>

using namespace physx;

class Physics
{
	friend class PhysScene;

	PxDefaultErrorCallback   defaultErrorCallback;
	PxDefaultAllocator       defaultAllocatorCallback;
	PxSimulationFilterShader defaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial*              defMaterial = nullptr;

	PxFoundation* foundation = nullptr;
	PxPhysics*    physics = nullptr;
	PxCooking*    cooking = nullptr;

	float physStep = 1.0f / 60.0f;
	float accum_dt = 0.0f;

	std::vector<PhysScene*> scenes;

public:

	void Init();
	PhysScene* CreateScene();
	void DestroyScene(PhysScene* scene);
	void Update(float dt);
	void Fetch();
};

extern Physics physics;