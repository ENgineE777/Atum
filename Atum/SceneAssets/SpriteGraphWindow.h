#pragma once

#ifdef EDITOR

#include <EUI.h>
#include "Support/Support.h"

class SpriteGraphAsset;

class SpriteGraphWindow : public EUIWidget::Listener
{
public:

	EUIButton*     btn_add_node;
	EUIButton*     btn_make_def;
	EUIButton*     btn_add_logical;
	EUIButton*     btn_add_group;
	EUIButton*     btn_del;
	EUICategories* panel_obj;

	SpriteGraphAsset* graph = nullptr;
	EUIWindow* wnd;

	static SpriteGraphWindow* instance;

	static void StartEdit(SpriteGraphAsset* graph);
	static void StopEdit();

	void Init();
	void Show(bool sh);

	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnWinClose(EUIWidget* sender) override;
};

#endif