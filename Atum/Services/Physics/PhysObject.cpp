#include "PhysObject.h"

bool PhysObject::IsSatic()
{
	return isStatic;
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
	if (isStatic)
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