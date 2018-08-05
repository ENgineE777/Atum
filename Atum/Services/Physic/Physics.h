
#pragma once

#include "PhysScene.h"
#include <vector>
#include "Services/Physic/Box2D/Box2D.h"

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

#ifdef PLATFORM_PC
	PxCooking*    cooking = nullptr;
#endif

	float physStep = 1.0f / 60.0f;
	float accum_dt = 0.0f;

	std::vector<PhysScene*> scenes;
	std::vector<b2World*> scenes2D;

	class StreamWriter : public PxOutputStream
	{
		FILE* file = nullptr;

	public:
		~StreamWriter()
		{
			if (file)
			{
				fclose(file);
			}
		}

		bool Prepere(const char* name)
		{
			file = fopen(name, "wb");

			return (file != nullptr);
		}

		virtual uint32_t write(const void* src, uint32_t count)
		{
			return (uint32_t)fwrite(src, count, 1, file);
		}
	};

	class StraemReader : public PxInputStream
	{
	public:
		Buffer buffer;
		virtual uint32_t read(void* dest, uint32_t count)
		{
			buffer.Read(dest, count);
			return count;
		}
	};

public:

	void Init();
#ifdef PLATFORM_PC
	void CookHeightmap(PhysHeightmap::Desc& desc, const char* name);
#endif
	PhysScene* CreateScene();
	b2World* CreateScene2D();
	void DestroyScene(PhysScene* scene);
	void DestroyScene2D(b2World* scene);
	void Update(float dt);
	void Fetch();
};

extern Physics physics;