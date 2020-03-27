
#pragma once

#include "PhysScene.h"
#include <vector>
#include "Services/File/FileInMemory.h"
using namespace physx;

/**
\ingroup gr_code_services_physic
*/

/**
\brief Physics

This is a manager of PhysScene objects.

*/

class Physics
{
	friend class PhysScene;
	friend class PhysObjectBase;

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
		FileInMemory buffer;
		virtual uint32_t read(void* dest, uint32_t count)
		{
			buffer.Read(dest, count);
			return count;
		}
	};

	vector<PhysObjectBase*> needed_to_delete;

public:

#ifndef DOXYGEN_SKIP
	void Init();
	void Update(float dt);
	void Fetch();
	void DeleteObjects();
#endif

#ifdef PLATFORM_PC
	void CookHeightmap(int width, int height, uint8_t* hmap, const char* name);
#endif

	/**
	\brief Creates a new physical scene

	\return Pointer to a PhysScene
	*/
	PhysScene* CreateScene();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script

	\
	*/
	void DestroyScene(PhysScene* scene);
};