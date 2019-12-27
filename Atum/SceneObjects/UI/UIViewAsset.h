#pragma once
#include "UIWidgetAsset.h"

/**
\page scene_assets_ui_view_asset Asset

UI view which is always a root widget

This class ::UIViewAsset is a representation on C++ side.

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
\brief Representation of UI view asset

UI view which is always a root widget

*/

class UIViewAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIViewAsset)

#ifndef DOXYGEN_SKIP

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;

	void Release() override;

#ifdef EDITOR
	SceneObject* CreateInstance(Scene* scene) override;

	static UIWidgetAsset* sel_ui_asset;
	static UIWidgetAsset* asset_to_copy;
	static int            popup_child_index;
	static UIWidgetAsset* popup_item;

	bool IsAsset() override;
	bool UseAseetsTree() override;
	void SetEditMode(bool ed) override;
	void PreapreAssetTree()  override;
	bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneObject* item, int prev_child_index, SceneObject* target, int child_index) override;
	void OnAssetTreeSelChange(SceneObject* item) override;
	void OnAssetTreePopupItem(int id) override;
	void OnAssetTreeRightClick(int x, int y, SceneObject* item, int child_index) override;
	void AddWidgetToTreeView(UIWidgetAsset* widget, void* parent_item);
	void ReCreteChilds(UIWidgetAsset* source, UIWidgetAsset* dest, bool childs_as_inst, bool create_item);
	void CheckProperties() override;
	bool IsInstance(UIWidgetAsset* node, UIWidgetAsset* source);
	void FillPopupCreateMenu(const char* name, int id);
#endif

#endif
};