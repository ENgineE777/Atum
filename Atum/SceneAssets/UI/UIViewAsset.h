#pragma once
#include "UIWidgetAsset.h"

class UIViewAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIViewAsset)

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;

	void Release() override;

#ifdef EDITOR
	SceneObject* CreateInstance() override;

	static UIWidgetAsset* sel_ui_asset;
	static UIWidgetAsset* asset_to_copy;
	static int            popup_child_index;
	static UIWidgetAsset* popup_item;

	bool UseAseetsTree() override;
	void SetEditMode(bool ed) override;
	void PreapreAssetTree()  override;
	bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index) override;
	void OnAssetTreeSelChange(SceneAsset* item) override;
	void OnAssetTreePopupItem(int id) override;
	void OnAssetTreeRightClick(int x, int y, SceneAsset* item, int child_index) override;
	void AddWidgetToTreeView(UIWidgetAsset* widget, void* parent_item);
	void ReCreteChilds(UIWidgetAsset* source, UIWidgetAsset* dest, bool childs_as_inst, bool create_item);
	void CheckProperties() override;
	bool IsInstance(UIWidgetAsset* node, UIWidgetAsset* source);
	void FillPopupCreateMenu(const char* name, int id);
#endif
};