#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_container_asset Asset

UI container that just holds another widgets as childs.

This class ::UIContainerAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------|--------------
x                 | X coordinate of a position
y                 | Y coordinate of a position
horz_align        | Horizontal aligment of a widget
vert_align        | Vertical aligment of a widget
width             | Width of a widget
height            | Height of a widget
horz_size         | Type of width of a widget
vert_size         | Type of height of a widget
anchorn_x         | X coordinate of anchorn in absolute units
anchorn_y         | Y coordinate of anchorn in absolute units
left_padding      | Left padding of a widget
top_padding       | Top padding of a widget
right_padding     | Right padding of a widget
bottom_padding    | Bottom padding of a widget
rotate            | Rotation a widget
color             | Color of a widget
alpha             | Transparancy of a widget
scale_childs      | Should be childs sacled in case size of a widget was changed
clip_childs       | Should be childs clipped by size of a widget

*/


/**
\ingroup gr_code_scene_assets_ui
*/

/**
\brief Representation of UI container widget instance

UI container that just holds another widgets as childs.

*/

class UIContainerAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIContainerAsset)

#ifndef DOXYGEN_SKIP

	void Init() override;

	void Draw(float dt) override;

#endif
};

/**
\page scene_assets_ui_container_inst Instance

UI container that just holds another widgets as childs.

This class ::UIContainerAssetInst is a representation on C++ side.

This class Script::UIContainer is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
color             | Color of a widget
alpha             | Transparancy of a widget

*/


/**
\ingroup gr_code_scene_objects_ui
*/

/**
\brief Representation of UI container widget instance

UI container that just holds another widgets as childs.

This class Script::UIContainer is a representation in a script.

*/

class UIContainerAssetInst : public UIContainerAsset
{
public:

	META_DATA_DECL(UIContainerAssetInst)

#ifndef DOXYGEN_SKIP

	void BindClassToScript() override;
#ifdef EDITOR
	static UIContainerAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif

#endif
};