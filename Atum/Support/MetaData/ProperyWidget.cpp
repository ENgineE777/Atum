#include "ProperyWidget.h"

void ProperyWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	panel = new EUIPanel(parent, 2, 0, 200, 30);
	panel->SetText(labelName);
	label = new EUILabel(panel, labelName, 5, 5, 85, 20);

	parent->RegisterChildInCategory(catName, panel, true);
}

void ProperyWidget::Show(bool show)
{
	panel->Show(show);
}

void ProperyWidget::Release()
{
	if (panel)
	{
		delete panel;
	}

	delete this;
}