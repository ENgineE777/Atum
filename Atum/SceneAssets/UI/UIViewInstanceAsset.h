#pragma once
#include "UIWidgetAsset.h"

class UIViewInstanceAsset : public UIWidgetAsset
{
public:
	META_DATA_DECL(UIViewInstanceAsset)

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;
#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
	void Release() override;
};

class UIViewInstance : public UIViewInstanceAsset
{
public:
	META_DATA_DECL(UIViewInstance)

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt) override;
#ifdef EDITOR
	void SetEditMode(bool ed) override;
#endif
};