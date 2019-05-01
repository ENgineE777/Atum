#pragma once
#include "UIWidgetAsset.h"

class Font;

/**
\page scene_assets_ui_label_asset Asset

Image

*/


/**
\ingroup gr_code_scene_assets_ui
*/

class UILabelAsset : public UIWidgetAsset
{
public:

	Font* font = nullptr;
	string font_name;
	string text;
	int font_height;
	Align textHorzAlign = align_left;
	Align textVertAlign = align_top;

	META_DATA_DECL(UILabelAsset)

	void Init() override;
	void ApplyProperties() override;

	void SetText(string& text);
	void Draw(float dt) override;
};

/**
\page scene_assets_ui_label_inst Instance

Label

*/


/**
\ingroup gr_code_scene_objects_ui
*/

class UILabelAssetInst : public UILabelAsset
{
public:

	META_DATA_DECL(UILabelAssetInst)

	void BindClassToScript() override;
#ifdef EDITOR
	static UILabelAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};