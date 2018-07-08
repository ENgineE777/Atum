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

	META_DATA_DECL(UILabelAsset)

	void Init() override;
	void ApplyProperties() override;

	void Draw(float dt) override;
};

class UILabelAssetInst : public UILabelAsset
{
public:

	META_DATA_DECL(UILabelAssetInst)

	void BindClassToScript() override;
	void SetText(string& text);
#ifdef EDITOR
	static UILabelAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};