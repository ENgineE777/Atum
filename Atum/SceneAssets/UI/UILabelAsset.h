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

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};

class UILabelAssetInst : public UILabelAsset
{
public:

	META_DATA_DECL(UILabelAssetInst)

#ifdef EDITOR
	static UILabelAssetInst* temp;
	void SetEditMode(bool ed) override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};