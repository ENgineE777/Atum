
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

#include "PxPhysicsAPI.h"
#include "PhysObjectBase.h"

/**
\ingroup gr_code_services_physic
*/

/**
\brief PhysObject

Just a physical box.

*/

using namespace physx;

class PhysObject : public PhysObjectBase
{
	friend class PhysScene;

public:

	enum BodyType
	{
		Static = 0 /*!< Hardware button in pressed state */,
		Dynamic /*!< Hardware button in pressed state */,
		DynamicCCD /*!< Hardware button in pressed state */,
		Kinetic /*!< Hardware button in pressed state */,
		Trigger /*!< Hardware button in pressed state */
	};

	/**
	\brief Set active state of a object

	\param[in] set Active state
	*/
	void SetActive(bool set);

	/**
	\brief Check if a object is active

	\return Returns active state
	*/
	bool IsActive();

	/**
	\brief Set pointer to a user data

	\param[in] data Pointer to a user data
	*/
	void SetUserData(void* data);

	/**
	\brief Return pointer to a user data

	\return Pointer to a user data
	*/
	void* GetUserData();

	/**
	\brief Locking and unlocking rotation

	\param[in] set Should be rotation locked or not
	*/
	void SetFixedRotation(bool set);

	/**
	\brief Get type of a object

	\return Type of a object
	*/
	BodyType GetType();

	/**
	\brief Set belonging to a physical group

	\param[in] group Physical group
	*/
	void SetGroup(int group);

	/**
	\brief Set current transform

	\param[in] mat Matrix with transform
	*/
	void SetTransform(Matrix& mat);

	/**
	\brief Get current transform

	\param[out] mat Output matrix
	*/
	void GetTransform(Matrix& mat);

	/**
	\brief Apply force to point of a object

	\param[in] pos Position of applying of a force
	\param[in] force Strength of a force
	*/
	void AddForceAt(Vector3 pos, Vector3 force);

	/**
	\brief PhysObject should released only via this mehod
	*/
	void Release();

protected:
	BodyType body_type;
	PxHeightField* heightField = nullptr;
	PxRigidActor* actor = nullptr;
	bool is_active = true;
	void ActualRelease() override;
};
