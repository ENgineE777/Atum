
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_GenericMarker Generic Marker

Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
be finded via call of Scene::GetGroup

This class ::GenericMarker is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
scene_group       | Name of group which will be used in registration via call Scene::AddToGroup

*/


/**
\ingroup gr_code_scene_objects_3D
*/

/**
\brief Representation of a generic marker in 3D space

Just generic marker in 3D space that has position in 3D space and can belong to particual scene group and can
be finded via call of Scene::GetGroup

*/

class GenericMarker : public SceneObject
{
public:

	/**
	\brief Transform of a marker
	*/
	Matrix transform;

	/**
	\brief Name of group which will be used in registration via class Scene::AddToGroup
	*/
	string scene_group;

#ifndef DOXYGEN_SKIP
	META_DATA_DECL(GenericMarker)

	Matrix& Trans() override;
	bool Is3DObject() override;
	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);
#endif
};
