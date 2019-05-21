
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Phys2DComp.h"

/**
\page scene_object_2D_Trigger2D Trigger2D

Whenever a physical object starts intersection with trigger script callback OnContactStart is called.
Script callback OnContactEnd called after object leaves area of a trigger.

This class ::Trigger2D is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
x                 | X coordinate of position of a trigger
y                 | Y coordinate of position of a trigger
width             | Width of a trigger
height            | Height of a trigger

*/

/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of a triger in 2D sprites

Whenever a physical object starts intersection with trigger script callback OnContactStart is called.
Script callback OnContactEnd called after object leaves area of a trigger.

*/

class Trigger2D : public SceneObject
{
public:
	META_DATA_DECL(Trigger2D)

	/**
	\brief Transform data of a triger.
	*/

	Transform2D trans;

#ifndef DOXYGEN_SKIP

	PhysScene::BodyUserData body;

	virtual ~Trigger2D() = default;

	void Init() override;

	bool Play() override;
	void Release() override;

#ifdef EDITOR
	void Set2DPos(Vector2 pos) override;
	void EditorDraw(float dt);
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
#endif

#endif
};
