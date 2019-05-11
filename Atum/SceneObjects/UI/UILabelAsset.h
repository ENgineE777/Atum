#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_label_asset Asset

UI text label.

This class ::UILabelAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------|--------------
font_name         | Name of font
font_height       | Font geight in logical units
text              | Displayed text
text_horz_align   | Horizontal aligment of a text
text_vert_align   | Vertical aligment of a text
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
\brief Representation of UI text label asset

UI text label.

*/

class UILabelAsset : public UIWidgetAsset
{
public:

	/**
	\brief Name of font
	*/
	string font_name;

	/**
	\brief Displayed text
	*/
	string text;

	/**
	\brief Font geight in logical units
	*/
	int font_height;

	/**
	\brief Horizontal aligment of a text
	*/
	Align textHorzAlign = align_left;

	/**
	\brief Vertical aligment of a text
	*/
	Align textVertAlign = align_top;

#ifndef DOXYGEN_SKIP

	Font* font = nullptr;

	META_DATA_DECL(UILabelAsset)

	void Init() override;
	void ApplyProperties() override;

	void SetText(string& text);
	void Draw(float dt) override;
#endif
};

/**
\page scene_assets_ui_label_inst Instance

UI text label.

This class ::UILabelAssetInst is a representation on C++ side.

This class Script::UILabel is a representation in a script.

Parameters
----------

Name              | Description
------------------|--------------
text              | Displayed text
color             | Color of a widget
alpha             | Transparancy of a widget

*/


/**
\ingroup gr_code_scene_objects_ui
*/

/**
\brief Representation of UI text label instance

UI text label.

This class Script::UILabel is a representation in a script.

*/

class UILabelAssetInst : public UILabelAsset
{
public:

	META_DATA_DECL(UILabelAssetInst)

#ifndef DOXYGEN_SKIP

	void BindClassToScript() override;
#ifdef EDITOR
	static UILabelAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif

#endif
};