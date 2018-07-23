#pragma once
#include "UIWidgetAsset.h"

class Font;

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