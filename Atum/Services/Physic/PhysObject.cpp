#include "PhysObject.h"
#include "PhysScene.h"

PhysObject::BodyType PhysObject::GetType()
{
	return body_type;
}

void PhysObject::SetActive(bool set)
{
	if (set == is_active)
	{
		return;
	}

	is_active = set;

	actor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !set);

	PxShape* shape;
	actor->getShapes(&shape, 1);

	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, set);
}

bool PhysObject::IsActive()
{
	return is_active;
}

void PhysObject::SetUserData(void* data)
{
	actor->userData = data;
}

void* PhysObject::GetUserData()
{
	return actor->userData;
}

void PhysObject::SetFixedRotation(bool set)
{
	((PxRigidDynamic*)actor)->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
}

void PhysObject::SetGroup(int group)
{
	PxShape* shape;
	actor->getShapes(&shape, 1);

	PhysScene::SetShapeGroup(shape, group);
}

void PhysObject::SetTransform(Matrix& mat)
{
	PxMat33 m;
	m.column0 = PxVec3(mat.Vx().x, mat.Vx().y, mat.Vx().z);
	m.column1 = PxVec3(mat.Vy().x, mat.Vy().y, mat.Vy().z);
	m.column2 = PxVec3(mat.Vz().x, mat.Vz().y, mat.Vz().z);

	PxTransform pT;
	pT.q = PxQuat(m);
	pT.p = PxVec3(mat.Pos().x, mat.Pos().y, mat.Pos().z);

	if (body_type == Kinetic)
	{
		PxSceneWriteLock scopedLock(*actor->getScene());
		((PxRigidDynamic*)actor)->setKinematicTarget(pT);
	}
	else
	{
		actor->setGlobalPose(pT, true);
	}
}

void PhysObject::GetTransform(Matrix& mat)
{
	PxTransform pT = actor->getGlobalPose();
	PxMat33 m = PxMat33(pT.q);

	mat.Vx() = Vector(m.column0.x, m.column0.y, m.column0.z);
	mat.Vy() = Vector(m.column1.x, m.column1.y, m.column1.z);
	mat.Vz() = Vector(m.column2.x, m.column2.y, m.column2.z);

	mat.Pos() = Vector(pT.p.x, pT.p.y, pT.p.z);
}

void PhysObject::AddForceAt(Vector pos, Vector force)
{
	if (body_type != Dynamic && body_type != DynamicCCD)
	{
		return;
	}

	PxRigidBodyExt::addForceAtPos(*((PxRigidBody*)actor), PxVec3(force.x, force.y, force.z), PxVec3(pos.x, pos.y, pos.z), PxForceMode::eIMPULSE, true);
}

void PhysObject::Release()
{
	actor->release();
	delete this;
}