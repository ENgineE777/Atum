#include "PhysHeightmap.h"

void PhysHeightmap::Release()
{
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