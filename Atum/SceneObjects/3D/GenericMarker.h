
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_GenericMarker Generic Marker

Generic Marker

*/


/**
\ingroup gr_code_scene_objects_3D
*/

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
