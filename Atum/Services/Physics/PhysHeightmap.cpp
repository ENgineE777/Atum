#include "PhysHeightmap.h"

void PhysHeightmap::Release()
{
	actor->release();
	heightField->release();
	delete[] samples;

	delete this;
}