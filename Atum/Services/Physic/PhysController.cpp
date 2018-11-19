#include "PhysController.h"
#include "PhysObject.h"
#include "Services/Scene/SceneObject.h"
#include "SceneObjects/2D/Phys2DComp.h"


PX_INLINE void addForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{
	/*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
	{
	Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
	"PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
	return;
	}*/

	const PxTransform globalPose = body.getGlobalPose();
	const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const PxVec3 torque = (pos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

static void addForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true)
{
	//transform pos to world space
	const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	addForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

void PhysController::onShapeHit(const PxControllerShapeHit& hit)
{
	if (hit.shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
	{
		Phys2DCompInst::BodyUserData* udataA = static_cast<Phys2DCompInst::BodyUserData*>(hit.shape->getActor()->userData);
		Phys2DCompInst::BodyUserData* udataB = static_cast<Phys2DCompInst::BodyUserData*>(controller->getUserData());

		if (udataA && udataB)
		{
			if (udataA->object->OnContact(udataA->index, udataB->object, udataB->index))
			{
				//contact->SetEnabled(false);
			}
		}
	}

	return;

	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;

		if (0)
		{
			const PxVec3 p = actor->getGlobalPose().p + hit.dir * 10.0f;

			PxShape* shape;
			actor->getShapes(&shape, 1);
			PxRaycastHit newHit;
			PxU32 n = PxShapeExt::raycast(*shape, *shape->getActor(), p, -hit.dir, 20.0f, PxHitFlag::ePOSITION, 1, &newHit);
			if (n)
			{
				// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
				// useless stress on the solver. It would be possible to enable/disable vertical pushes on
				// particular objects, if the gameplay requires it.
				const PxVec3 upVector = hit.controller->getUpDirection();
				const PxF32 dp = hit.dir.dot(upVector);
				//		shdfnd::printFormatted("%f\n", fabsf(dp));
				if (fabsf(dp)<1e-3f)
					//		if(hit.dir.y==0.0f)
				{
					const PxTransform globalPose = actor->getGlobalPose();
					const PxVec3 localPos = globalPose.transformInv(newHit.position);
					::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
				}
			}
		}

		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);
		//		shdfnd::printFormatted("%f\n", fabsf(dp));
		if (fabsf(dp)<1e-3f)
			//		if(hit.dir.y==0.0f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
		}
	}
}

PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	Phys2DCompInst::BodyUserData* udataA = static_cast<Phys2DCompInst::BodyUserData*>(controller->getUserData());
	Phys2DCompInst::BodyUserData* udataB = static_cast<Phys2DCompInst::BodyUserData*>(actor.userData);

	if (udataA && udataB)
	{
		udataA->object->OnContact(udataA->index, udataB->object, udataB->index);
		udataB->object->OnContact(udataB->index, udataA->object, udataA->index);
	}

	if (udataB)
	{
		if (udataB->body->GetType() == PhysObject::Kinetic)
		{
			return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
		}
	}

	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysController::getBehaviorFlags(const PxObstacle& obstacle)
{
	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxQueryHitType::Enum PhysController::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	Phys2DCompInst::BodyUserData* udataA = static_cast<Phys2DCompInst::BodyUserData*>(GetUserData());
	Phys2DCompInst::BodyUserData* udataB = static_cast<Phys2DCompInst::BodyUserData*>(actor->userData);

	if (udataA && udataB)
	{
		if (!udataB->body->IsActive())
		{
			return PxQueryHitType::eNONE;
		}
		else
		{
			if (StringUtils::IsEqual(udataA->object->class_name, "Triger2D") ||
			    udataA->body && udataA->body->GetType() == PhysObject::Trigger ||
			    StringUtils::IsEqual(udataB->object->class_name, "Triger2D") ||
			    udataB->body && udataB->body->GetType() == PhysObject::Trigger)
			{
				udataA->object->OnContact(udataA->index, udataB->object, udataB->index);
				udataB->object->OnContact(udataB->index, udataA->object, udataA->index);
			}
		}
	}

	return PxQueryHitType::eTOUCH;
}

PxQueryHitType::Enum PhysController::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	return PxQueryHitType::eTOUCH;
}

void PhysController::SetActive(bool set)
{
	controller->getActor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !set);
}

bool PhysController::IsActive()
{
	return (((uint32_t)controller->getActor()->getActorFlags() & PxActorFlag::eDISABLE_SIMULATION) == 0);
}

void PhysController::SetUserData(void* data)
{
	controller->setUserData(data);
}

void* PhysController::GetUserData()
{
	return controller->getUserData();
}

float PhysController::GetHeight()
{
	return height;
}

bool PhysController::IsColliding(CollideType type)
{
	PxControllerState cctState;
	controller->getState(cctState);

	if ((cctState.collisionFlags & type) == 0)
	{
		int k = 0;
		k++;
	}

	return (cctState.collisionFlags & type) != 0;

}

void PhysController::Move(Vector dir)
{
	PxControllerFilters filters = PxControllerFilters();
	filters.mFilterCallback = this;

	const PxU32 flags = controller->move(PxVec3(dir.x, dir.y, dir.z), 0.0001f, 1.0f/60.0f, filters, NULL);
}

void PhysController::SetPosition(Vector pos)
{
	controller->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

void PhysController::GetPosition(Vector& pos)
{
	PxExtendedVec3 cpos = controller->getFootPosition();
	pos = Vector((float)cpos.x, (float)cpos.y, (float)cpos.z);
}

void PhysController::Release()
{
	controller->release();
	delete this;
}