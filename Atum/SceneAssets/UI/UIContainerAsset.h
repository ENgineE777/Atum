#pragma once
#include "UIWidgetAsset.h"

class Font;

class UIContainerAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIContainerAsset)

	void Init() override;

	void Draw(float dt) override;
};

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