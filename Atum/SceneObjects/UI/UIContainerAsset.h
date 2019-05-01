#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_container_asset Asset

Container

*/


/**
\ingroup gr_code_scene_assets_ui
*/

class UIContainerAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIContainerAsset)

	void Init() override;

	void Draw(float dt) override;
};

/**
\page scene_assets_ui_container_inst Instance

Container

*/


/**
\ingroup gr_code_scene_objects_ui
*/

class UIContainerAssetInst : public UIContainerAsset
{
public:

	META_DATA_DECL(UIContainerAssetInst)

	void BindClassToScript() override;
#ifdef EDITOR
	static UIContainerAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};