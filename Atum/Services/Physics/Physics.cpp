#include "Physics.h"

Physics physics;

#ifdef PLATFORM_ANDROID
extern "C"
{
	int android_getCpuCount()
	{
		return 1;
	}
}
#endif

void Physics::Init()
{
	PxTolerancesScale tolerancesScale;

	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, defaultAllocatorCallback, defaultErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale, true);

#ifdef PLATFORM_PC
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(tolerancesScale));
#endif

	defMaterial = physics->createMaterial(0.5f, 0.5f, 0.95f);
}

#ifdef PLATFORM_PC
void Physics::CookHeightmap(PhysHeightmap::Desc& desc, const char* name)
{
	PxHeightFieldSample* samples = new PxHeightFieldSample[desc.width * desc.height];

	for (int x = 0; x < desc.width; x++)
		for (int y = 0; y < desc.height; y++)
		{
			samples[x + y * desc.width].height = PxI16(desc.hmap[((x)* desc.width + y)]);
			samples[x + y * desc.width].setTessFlag();
			samples[x + y * desc.width].materialIndex0 = 1;
			samples[x + y * desc.width].materialIndex1 = 1;
		}

	PxHeightFieldDesc heightFieldDesc;

	heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightFieldDesc.nbColumns = desc.width;
	heightFieldDesc.nbRows = desc.height;
	heightFieldDesc.samples.data = samples;
	heightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);

	StraemWriter writer;
	if (writer.Prepere(name))
	{
		cooking->cookHeightField(heightFieldDesc, writer);
	}
}
#endif

PhysScene* Physics::CreateScene()
{
	PhysScene* scene = new PhysScene();

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	}

	if (!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	}

	sceneDesc.isValid();

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
			PhysScene* scene = scenes[i];

			if (!scene->needFetch)
			{
			}
			scenes[i]->Simulate(physStep);
			scenes[i]->needFetch = true;
		}

		accum_dt -= physStep;
	}
}

void Physics::Fetch()
{
	for (int i = 0; i < scenes.size(); i++)
	{
		PhysScene* scene = scenes[i];

		if (!scene->needFetch)
		{
			continue;
		}

		scene->FetchResults();
		scene->needFetch = false;
	}
}