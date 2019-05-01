#pragma once
#include "UIWidgetAsset.h"

/**
\page scene_assets_ui_image_asset Asset

Image

*/


/**
\ingroup gr_code_scene_assets_ui
*/


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

/**
\page scene_assets_ui_image_inst Instance

Image

*/


/**
\ingroup gr_code_scene_objects_ui
*/

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