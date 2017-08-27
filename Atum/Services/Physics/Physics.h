
#pragma once

#include "PxPhysicsAPI.h"
#include <vector>

using namespace physx;

class PhysObject
{

};

class PhysScene
{
public:
	PxScene* scene;
	PxControllerManager* manager;
	void Simulate(float dt);
	bool FetchResults();
	void Release();
};

class Controller
{

};

class Physics
{
	PxDefaultErrorCallback   defaultErrorCallback;
	PxDefaultAllocator       defaultAllocatorCallback;
	PxSimulationFilterShader defaultFilterShader;

public:

	PxFoundation* foundation = NULL;
	PxPhysics*    physics = NULL;
	PxCooking*    cooking = NULL;

	float physStep = 1.0f / 60.0f;
	float accum_dt = 0.0f;

	std::vector<PhysScene*> scenes;

public:

	void Init();
	PhysScene* CreateScene();
	void DestroyScene(PhysScene* scene);
	void Update(float dt);


};

extern Physics physics;