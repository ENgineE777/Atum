
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteTileAsset.h"
#include "SpriteInst.h"

/**
\page scene_assets_2D_SpriteTileInst Instance

Instance holds array of sprite instances. This array can be binded into a script array.

To add new sprite instance firstly scene object needed to be selected. After that press hot key 'O'
to add new sprite inctance into center of screen. If another sprite instace is selected then pressing
hot key 'P' will add another instance of a sprite near previosly selected one. Press hot key 'I' to
delete selected instamce of a sprite.

Scene object supporting muliselection. That means holding Left Alt and dragging mouse after left mouse click
will select multiple scene instances. Pressing hot key 'I' will fill selection by instance. Pressing
hot key 'P' will delete evry instances inside rectangular area.

![Caption text](../../../Doxygen/images/NineTiles.jpg "Image")

This class ::SpriteTileInst is a representation on C++ side.

Instance property parameters
----------------------------

Name              | Description
------------------| -------------
x                 | X coordinate of a position
y                 | Y coordinate of a position
visible           | If instance is visible. 1 means visible. 0 means invicible
color             | Color of a instance
alpha             | Transparency of a instance

Parameters
----------

Name              | Description
------------------| -------------
axis_scale        | Scale of a axis
Depth             | Z depth
draw_level        | Draw priority
inst              | Array of instances of a sprite assets


*/


/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of instance of sprite nine tile asset

Instance holds array of sprite instances. This array can be binded into a script array.

*/

class SpriteTileInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteTileInst)

#ifndef DOXYGEN_SKIP

	virtual ~SpriteTileInst() = default;

	void Init() override;
	void ApplyProperties() override;
	bool Play() override;
	void Draw(float dt) override;

#ifdef EDITOR
	bool IsOccupied(Vector2 pos);
	void CalcIndices();
#endif

#endif
};
