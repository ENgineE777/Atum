#pragma once
#include "UIWidgetAsset.h"

class UIImageAsset : public UIWidgetAsset
{
public:

	META_DATA_DECL(UIImageAsset)

	Sprite::Data sprite;
	static Sprite::FrameState state;

	virtual ~UIImageAsset();

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;
	void Draw(float dt) override;

#ifdef EDITOR
	void Copy(SceneObject* src) override;
#endif
};

class UIImageAssetInst : public UIImageAsset
{
public:

	META_DATA_DECL(UIImageAssetInst)

#ifdef EDITOR
	static UIImageAssetInst* temp;
	void StoreProperties() override;
	void RestoreProperties() override;
#endif
};