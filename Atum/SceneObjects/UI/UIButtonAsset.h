#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_button_asset Asset

Button

*/


/**
\ingroup gr_code_scene_assets_ui
*/

class UIButtonAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIButtonAsset)

	void Init() override;

	void Draw(float dt) override;
};

/**
\page scene_assets_ui_button_inst Instance

Button

*/


/**
\ingroup gr_code_scene_objects_ui
*/

class UIButtonAssetInst : public UIButtonAsset
{
public:

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
};