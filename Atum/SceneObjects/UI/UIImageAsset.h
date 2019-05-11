#pragma once
#include "UIWidgetAsset.h"

/**
\page scene_assets_ui_image_asset Asset

UI image widget.

This class ::UIImageAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------|--------------
image             | Sprite data of image
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
\brief Representation of UI image widget asset
UI image widget.

*/

class UIImageAsset : public UIWidgetAsset
{
public:

	/**
	\brief Sprite data of image
	*/

	Sprite::Data sprite;

#ifndef DOXYGEN_SKIP

	META_DATA_DECL(UIImageAsset)

	Sprite::FrameState frame_state;

	virtual ~UIImageAsset();

	void Init() override;
	void Draw(float dt) override;

#endif
};

/**
\page scene_assets_ui_image_inst Instance

UI image widget.

This class ::UIImageAssetInst is a representation on C++ side.

This class Script::UIImage is a representation in a script.

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
\brief Representation of UI image widget instance

UI image widget.

This class Script::UIImage is a representation in a script.

*/

class UIImageAssetInst : public UIImageAsset
{
public:

	META_DATA_DECL(UIImageAssetInst)

#ifndef DOXYGEN_SKIP

	void BindClassToScript() override;
	void SetSpriteFromAsset(string& asset);

#ifdef EDITOR
	static UIImageAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif

#endif
};