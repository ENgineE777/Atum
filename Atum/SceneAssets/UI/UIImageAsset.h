#pragma once
#include "UIWidgetAsset.h"

class UIImageAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIImageAsset)

	Sprite::Data sprite;
	Sprite::FrameState frame_state;

	virtual ~UIImageAsset();

	void Init() override;
	void Draw(float dt) override;
};

class UIImageAssetInst : public UIImageAsset
{
public:

	META_DATA_DECL(UIImageAssetInst)

	void BindClassToScript() override;
	void SetSpriteFromAsset(string& asset);

#ifdef EDITOR
	static UIImageAssetInst* temp;
	bool AddedToTreeByParent() override;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};