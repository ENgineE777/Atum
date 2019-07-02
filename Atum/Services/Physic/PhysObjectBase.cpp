
#include "Services/Core/Core.h"

void PhysObjectBase::Release()
{
	core.physics.needed_to_delete.push_back(this);
}