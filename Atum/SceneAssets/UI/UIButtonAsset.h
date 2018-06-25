#pragma once
#include "UIWidgetAsset.h"

class Font;

class UIButtonAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIButtonAsset)

	void Init() override;

	void Draw(float dt) override;

#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};

class UIButtonAssetInst : public UIButtonAsset
{
public:

	int alias_rotate_x;
	int alias_rotate_y;
	int alias_fire;
	int fired = 0;

	META_DATA_DECL(UIButtonAssetInst)

	void Init() override;

	void Draw(float dt) override;

#ifdef EDITOR
	static UIButtonAssetInst* temp;
	void SetEditMode(bool ed) override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};