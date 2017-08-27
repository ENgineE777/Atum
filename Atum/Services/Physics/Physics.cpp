#include "Physics.h"

Physics physics;

void PhysScene::Simulate(float dt)
{
	scene->simulate(dt);
}

bool PhysScene::FetchResults()
{
	return scene->fetchResults();
}

void PhysScene::Release()
{
	scene->release();
	delete this;
}

void Physics::Init()
{
	PxTolerancesScale tolerancesScale;

	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale, true);
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(tolerancesScale));
}

PhysScene* Physics::CreateScene()
{
	PhysScene* scene = new PhysScene();

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = defaultFilterShader;
	}

	scene->scene = physics->createScene(sceneDesc);
	scene->manager = PxCreateControllerManager(*scene->scene);

	scenes.push_back(scene);

	return scene;
}

void Physics::DestroyScene(PhysScene* scene)
{
	for (int i=0; i<scenes.size(); i++)
	{
		if (scenes[i] == scene)
		{
			scenes.erase(scenes.begin() + i);
			scene->Release();
		}
	}
}

void Physics::Update(float dt)
{
	accum_dt += dt;

	if (accum_dt > 0.5f)
	{
		accum_dt = 0.5f;
	}

	if (accum_dt > physStep)
	{
		for (int i = 0; i < scenes.size(); i++)
		{
			if (scenes[i]->FetchResults())
			{
				scenes[i]->Simulate(physStep);
			}
		}

		accum_dt -= physStep;
	}
}
