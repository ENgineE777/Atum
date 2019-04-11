#include "Physics.h"
#include "Services/Render/Render.h"
#include "SceneObjects/2D/Sprite.h"

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

	defMaterial = physics->createMaterial(0.5f, 0.5f, 0.0005f);
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

	StreamWriter writer;
	if (writer.Prepere(name))
	{
		cooking->cookHeightField(heightFieldDesc, writer);
	}
}
#endif

physx::PxFilterFlags CollisionFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                           physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                           physx::PxPairFlags& retPairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	retPairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
	return PxFilterFlag::eDEFAULT;
}

PhysScene* Physics::CreateScene()
{
	PhysScene* scene = new PhysScene();

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.simulationEventCallback = scene;
	sceneDesc.filterShader = CollisionFilterShader;

	if (!sceneDesc.cpuDispatcher)
	{
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	}

	sceneDesc.isValid();

	scene->scene = physics->createScene(sceneDesc);
	scene->manager = PxCreateControllerManager(*scene->scene);
	scene->scene->setFlag(physx::PxSceneFlag::eENABLE_CCD, true);

	//scene->SetVisualization(true);

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
			break;
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
		for (auto scene : scenes)
		{
			if (!scene->needFetch)
			{
			}

			scene->Simulate(physStep);
			scene->needFetch = true;
		}

		accum_dt -= physStep;
	}

	/*for (auto scene : scenes)
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		Vector2 cam_pos;
		cam_pos.x = Sprite::cam_pos.x / 50.0f;//* scale - render.GetDevice()->GetWidth() * 0.5f;
		cam_pos.y = Sprite::cam_pos.y / 50.0f;//* scale - 512 * scale;

		Matrix view;
		view.BuildView({ cam_pos.x, -cam_pos.y, -512.0f / 20.25f }, { cam_pos.x, -cam_pos.y, 0.0f }, Vector(0, 1, 0));

		render.SetTransform(Render::View, view);

		Matrix proj;
		proj.BuildProjection(45.0f * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
		render.SetTransform(Render::Projection, proj);

		scene->DrawVisualization();
	}*/
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