
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteInst.h"

/**
\page scene_assets_2D_SpriteGraphInst Instance

Instance holds array of sprite graph instances. This array can be binded into a script array.

To add new sprite graph instance firstly scene object needed to be selected. After that press hot key 'P'
to add new sprite graph instance into center of screen. If another node is selected then pressing hot key
'O' will add copy of a sprite near previosly selected one. Press hot key 'I' to delete selected instance of a sprite graph.

This class ::SpriteGraphInst is a representation on C++ side.

This class Script::Graph2D is a representation in a script.

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
Depth             | Z depth
hack_height       | Addition height applyed fr every instance
draw_level        | Draw priority
inst              | Array of instances of a sprite assets

*/


/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of instance of sprite graph asset

Instance holds array of sprite graph instances. This array can be binded into a script array.

This class Script::SpriteGraph is a representation in a script.

*/

class SpriteGraphInst : public SpriteInst
{
public:
	META_DATA_DECL(SpriteGraphInst)

#ifndef DOXYGEN_SKIP

	float hack_height = 0.0f;

	GET_ASSET_METHOD(SpriteGraphAsset)

	virtual ~SpriteGraphInst() = default;

	void BindClassToScript() override;

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;

	bool Play() override;

	void OnResize(int at, int delta) override;

	void PrepereInstaces();
#ifdef EDITOR
	void ShowProperties(bool show);
#endif

#endif
};
