#include "PhysController.h"

void PhysController::Move(Vector dir)
{
	const PxU32 flags = controller->move(PxVec3(dir.x, dir.y, dir.z), 0.01f, 1.0f/60.0f, PxControllerFilters(), NULL);
}

void PhysController::SetPosition(Vector& pos)
{
	controller->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
}

void PhysController::GetPosition(Vector& pos)
{
	PxExtendedVec3 cpos = controller->getFootPosition();
	pos = Vector(cpos.x, cpos.y, cpos.z);
}

void PhysController::Release()
{
	controller->release();
	delete this;
}