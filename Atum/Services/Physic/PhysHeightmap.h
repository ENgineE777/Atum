
#pragma once

#ifdef PLATFORM_IOS

#ifdef _DEBUG
#undef _DEBUG
#define DEBUG_WAS_DEFINED
#endif

#endif

#include "PxPhysicsAPI.h"

#ifdef PLATFORM_IOS

#ifdef DEBUG_WAS_DEFINED
#undef DEBUG_WAS_DEFINED
#define _DEBUG

#endif

#endif

#include "Support/Support.h"

/**
\ingroup gr_code_services_physic
*/

/**
\brief PhysHeightmap

Static height map.

*/

using namespace physx;

class PhysHeightmap
{
	friend class PhysScene;
	PxHeightField* heightField = nullptr;
	PxRigidActor*  actor = nullptr;
public:

	/**
	\brief PhysScene

	This is representation of a physical scene. Objecte to a scene are adding via proper
	methods of PhysScene.

	*/

	struct Desc
	{
		/** \brief Width of height map */
		int width;
		/** \brief Heighy of height map */
		int height;
		/** \brief Verical and horizontal scales */
		Vector2  scale;
#ifdef PLATFORM_PC
		uint8_t* hmap = nullptr;
#endif
	};

	/**
	\brief PhysHeightmap should released only via this mehod
	*/
	void Release();
};
