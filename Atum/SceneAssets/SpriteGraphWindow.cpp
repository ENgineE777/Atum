
#ifdef EDITOR

#include "SpriteGraphWindow.h"
#include "SpriteGraphAsset.h"

SpriteGraphWindow* SpriteGraphWindow::instance = nullptr;

void SpriteGraphWindow::StartEdit(SpriteGraphAsset* graph)
{
	if (!instance)
	{
		instance = new SpriteGraphWindow();
		instance->Init();
	}

	instance->graph = graph;
	instance->Show(true);
}

void SpriteGraphWindow::StopEdit()
{
	if (instance)
	{
		instance->graph = nullptr;
		instance->Show(false);
	}
}

void SpriteGraphWindow::Init()
{
	wnd = new EUIWindow("Graph Editor", EUIWindow::Popup, false, 10, 35, 320, 640);
	wnd->SetListener(-1, this, 0);

	EUILayout* wnd_lt = new EUILayout(wnd, true);
	EUIPanel* panel = new EUIPanel(wnd_lt, 0, 0, 100, 100);
	wnd_lt->SetChildSize(panel, 30, false);

	btn_add_node = new EUIButton(panel, "+ Node", 5, 5, 50, 20);
	btn_add_node->SetListener(-1, this, 0);

	btn_make_def = new EUIButton(panel, "MakeDef", 65, 5, 50, 20);
	btn_make_def->SetListener(-1, this, 0);

	/*btn_add_logical = new EUIButton(panel, "+ Logical", 125, 5, 50, 20);
	btn_add_logical->SetListener(-1, this, 0);

	btn_add_group = new EUIButton(panel, "+ Group", 185, 5, 50, 20);
	btn_add_group->SetListener(-1, this, 0);*/

	btn_del = new EUIButton(panel, "Delete", 245, 5, 50, 20);
	btn_del->SetListener(-1, this, 0);

	panel_obj = new EUICategories(wnd_lt, 0, 0, 200, 300);
}

void SpriteGraphWindow::Show(bool sh)
{
	wnd->Show(sh);
}

void SpriteGraphWindow::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == btn_add_node)
	{
		graph->CreateNode(SpriteGraphAsset::AnimNode);
	}

	if (sender == btn_make_def)
	{
		graph->MakeNodeAsDefault();
	}

	if (sender == btn_add_logical)
	{
		graph->CreateNode(SpriteGraphAsset::LogicNode);
	}

	if (sender == btn_add_group)
	{
		graph->CreateNode(SpriteGraphAsset::GroupNode);
	}

	if (sender == btn_del)
	{
		graph->Delete();
	}
}

void SpriteGraphWindow::OnWinClose(EUIWidget* sender)
{
	instance = nullptr;
}

#endif