#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_button_asset Asset

UI button that calls script method OnDown when button is clicked.
Also a button do not have any graphical representation.

This class ::UIButtonAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------|--------------
sound_name        | File name of a sound whic should be played on button click
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
\brief Representation of UI button widget asset

UI button that calls script method OnDown when button is clicked.
Also a button do not have any graphical representation.

*/

class UIButtonAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIButtonAsset)

	/**
	\brief Filename of a sound file.
	*/

	std::string sound_name;

	/**
	\brief Pointer to SoundInstance
	*/
	SoundInstance* sound_click = nullptr;

#ifndef DOXYGEN_SKIP

	void Init() override;

	void Release() override;

	void Draw(float dt) override;

#endif
};

/**
\page scene_assets_ui_button_inst Instance

UI button that calls script method OnDown when button is clicked.
Also a button do not have any graphical representation.

This class ::UIButtonAssetInst is a representation on C++ side.

This class Script::UIButton is a representation in a script.

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
\brief Representation of UI button widget instance

UI button that calls script method OnDown when button is clicked.
Also a button do not have any graphical representation.

This class Script::UIButton is a representation in a script.

*/

class UIButtonAssetInst : public UIButtonAsset
{
public:

#ifndef DOXYGEN_SKIP

	int alias_rotate_x;
	int alias_rotate_y;
	int alias_fire;

	META_DATA_DECL(UIButtonAssetInst)

	void Init() override;
	void Draw(float dt) override;

	void BindClassToScript() override;

#ifdef EDITOR
	static UIButtonAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif

#endif
};