#pragma once
#include "UIWidgetAsset.h"

class UIViewAsset : public UIWidgetAsset
{
public:

	static EUITreeView *  asset_treeview;
	static EUICategories* cat;
	static EUIEditBox*    objName;
	static UIWidgetAsset* sel_ui_asset;
	static UIWidgetAsset* asset_to_copy;
	static int            popup_child_index;
	static UIWidgetAsset* popup_item;

	META_DATA_DECL(UIViewAsset)

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;

	void Release() override;

	SceneObject* CreateInstance() override;

#ifdef EDITOR
	void SetEditMode(bool ed) override;
	bool PrepareWidgets(EUITreeView* asset_treeview, EUICategories* cat, EUIEditBox* obj_name) override;
	bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index) override;
	void OnAssetTreeSelChange(SceneAsset* item) override;
	void OnAssetTreePopupItem(int id) override;
	void OnAssetTreeRightClick(SceneAsset* item, int child_index) override;
	void AddWidgetToTreeView(UIWidgetAsset* widget, void* parent_item);
	void ReCreteChilds(UIWidgetAsset* source, UIWidgetAsset* dest, bool childs_as_inst);
	void CheckProperties() override;
	bool IsInstance(UIWidgetAsset* node, UIWidgetAsset* source);
	void FillPopupCreateMenu(const char* name, int id);
#endif
};