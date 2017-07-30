#include "ProperyWidget.h"

void ProperyWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	panel = new EUIPanel(parent, 0, 0, 200, 30);
	label = new EUILabel(panel, labelName, 5, 5, 85, 20);

	parent->RegisterChildInCategory(catName, panel);
}

void ProperyWidget::Show(bool show)
{
	panel->Show(show);
}