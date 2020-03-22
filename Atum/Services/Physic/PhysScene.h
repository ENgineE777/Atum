
#pragma once

#include "PhysObject.h"
#include "PhysController.h"
#include "PhysHeightmap.h"

/**
\ingroup gr_code_services_physic
*/

/**
\brief PhysScene

This is representation of a physical scene. Objects to a scene are adding via proper
methods of PhysScene.

*/

using namespace physx;

class PhysScene : public PxSimulationEventCallback
{
	friend class Physics;
	PxScene* scene = nullptr;
	bool needFetch = false;
	PxControllerManager* manager = nullptr;
	int debug_render = 0;

public:

	/**
	\brief BodyUserData

	This struct used as a wrapper around SceneObject and pointer sets to this wraper via SetUserData.

	*/

	struct BodyUserData
	{
		/** \brief Poninter to a scene object */
		class SceneObject* object = nullptr;

		/** \brief Index of a instance */
		int index = -1;

		/** \brief Pointer to a physical object */
		PhysObject* body = nullptr;

		/** \brief Ponter to a character controller */
		class PhysController* controller = nullptr;
	};

	/**
	\brief RaycastDesc

	This struct used in RayCast call for passing ray data and getting result of a ray cast.

	*/

	struct RaycastDesc
	{
		/** \brief Origin of a ray */
		Vector3 origin;

		/** \brief Direction of a ray. Should be normalized */
		Vector3 dir;

		/** \brief Length of a ray */
		float length;

		/** \brief Hit postion of a ray */
		Vector3 hitPos;

		/** \brief Hit normal of a ray */
		Vector3 hitNormal;

		/** \brief Group which should be tested */
		uint32_t group;

		/** \brief Pointer to a BodyUserData */
		BodyUserData* userdata = nullptr;
	};

	/**
	\brief Create a physical box in a scene

	\param[in] size Dimestion of a box
	\param[in] trans Transformation of a box
	\param[in] offset Offset of a orgin in local space
	\param[in] type Type of a body
	\param[in] group Belonging to a physical group

	\return Pointer to physical object
	*/
	PhysObject* CreateBox(Vector3 size, Matrix trans, Matrix offset, PhysObject::BodyType type, uint32_t group);

	/**
	\brief Create a character controller in a scene

	\param[in] desc Description of charater controller
	\param[in] group Belonging to a physical group

	\return Pointer to a character controller
	*/
	PhysController* CreateController(PhysControllerDesc& desc, uint32_t group);

	/**
	\brief Create a static heighfiled in a scene

	\param[in] desc Description of height map 
	\param[in] name Full path to a cooked heigh map
	\param[in] group Belonging to a physical group

	\return Pointer to a height map
	*/
	PhysHeightmap* CreateHeightmap(PhysHeightmap::Desc& desc, const char* name, uint32_t group);

	/**
	\brief Make ray cast

	\param[in] desc Description of a ray

	\return True will be return in case ray hitted something. Otherwise false will be returned 
	*/
	bool RayCast(RaycastDesc& desc);

#ifndef DOXYGEN_SKIP

	void SetVisualization(bool set);
	void DrawVisualization();

	void Simulate(float dt);
	void FetchResults();

	inline static void SetShapeGroup(PxShape* shape, uint32_t group)
	{
		PxFilterData data;
		data.word0 = group;
		shape->setSimulationFilterData(data);
		shape->setQueryFilterData(data);
	}

	void Release();

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {};
	void onWake(PxActor** actors, PxU32 count) {};
	void onSleep(PxActor** actors, PxU32 count) {};
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};

	static void HandleSceneObjectContact(class SceneObject* object, int index, class SceneObject* contact_object, int contact_index, const char* callback_name);
#endif
};