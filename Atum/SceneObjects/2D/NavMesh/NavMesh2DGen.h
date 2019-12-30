
#pragma once

#include "NavMesh2DBase.h"

class NavMesh2DGen : public NavMesh2DBase
{
public:

#ifndef DOXYGEN_SKIP
	META_DATA_DECL(NavMesh2DGen)

	float collider_radius = 1.0f;

	virtual ~NavMesh2DGen() = default;

	void Draw(float dt) override;

	void ConstructNavMesh() override;

#endif
};
