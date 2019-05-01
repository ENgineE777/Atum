#pragma once
#include "UIWidgetAsset.h"

class UIViewInstanceAsset : public UIWidgetAsset
{
public:
	META_DATA_DECL(UIViewInstanceAsset)

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;

	void Draw(float dt) override;
	void Release() override;

#ifdef EDITOR
	void SaveAssetData(JSONWriter& writer) override;
#endif
};

/**
\page scene_assets_ui_view_inst Instance

Label

*/


/**
\ingroup gr_code_scene_objects_ui
*/

class UIViewInstance : public UIViewInstanceAsset
{
public:
	META_DATA_DECL(UIViewInstance)

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;

#ifdef EDITOR
	bool IsAsset() override;
	bool UsingCamera2DPos() override;
	void AddWidgetToTreeView(EUITreeView* treeview, UIWidgetAsset* widget, void* parent_item);
	void AddChildsToTree(EUITreeView* treeview) override;
#endif
};