#include "PhysHeightmap.h"

void PhysHeightmap::Release()
{
	PhysObjectBase::Release();
}

void PhysHeightmap::ActualRelease()
{
	PxShape* shape;
	actor->getShapes(&shape, 1);
	actor->detachShape(*shape);

	if (actor)
	{
		actor->release();
	}

	if (heightField)
	{
		heightField->release();
	}

	delete this;
}