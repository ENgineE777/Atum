#pragma once
#include "UIWidgetAsset.h"

class Font;

class UIButtonAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIButtonAsset)

	void Init() override;

	void Draw(float dt) override;
};

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