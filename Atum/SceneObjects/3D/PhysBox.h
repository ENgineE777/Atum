
#pragma once

#include "Services/Scene/SceneObject.h"

/**
\page scene_object_3D_PhysBox Physic Box

Box can be static or dynamic. Dimentions are adjustable. Debug render used for
rendering in a scene. So this object is more for draft construction of levels.

This class ::PhysBox is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
color             | Display color of a box
SizeX             | Size along X axis of a box
SizeY             | Size along Y axis of a box
SizeZ             | Size along Z axis of a box
Is Static         | Set if object should be satic or dynamic

*/


/**
\ingroup gr_code_scene_objects_3D
*/

/**
\brief Representation of physical box in 3D space

Box can be static or dynamic. Dimentions are adjustable. Debug render used for
rendering in a scene. So this object is more for draft construction of levels.

*/

class PhysBox : public SceneObject
{
public:

	/**
	\brief Transform of a box.
	*/
	Matrix transform;

	/**
	\brief Size of a box along.
	*/

	Vector3 size;

	/**
	\brief Group of a body
	*/

	uint32_t phys_group;

	/**
	\brief Controls if box static or dynamic.
	*/

	bool isStatic;

	/**
	\brief Color of a box.
	*/

	Color color;


#ifndef DOXYGEN_SKIP

	META_DATA_DECL(PhysBox)

	PhysScene::BodyUserData body;

	virtual ~PhysBox() = default;

	void Load(JSONReader& reader) override;

	Matrix* Trans() override;
	bool Is3DObject() override;
	void Init() override;
	void Draw(float dt);

	bool Play() override;
	void Release() override;

#ifdef EDITOR
	bool CheckSelection(Vector2 ms, Vector3 start, Vector3 dir) override;
#endif

#endif
};
