
#pragma once

#include "Services/Scene/SceneObject.h"

class GenericMarker : public SceneObject
{
public:

	Matrix transform;
	META_DATA_DECL(GenericMarker)

	string scene_group;

	Matrix& Trans() override;
	bool Is3DObject() override;
	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);
};
