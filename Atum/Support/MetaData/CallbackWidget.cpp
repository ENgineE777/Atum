
#include "CallbackWidget.h"

void CallbackWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	openBtn = new EUIButton(panel, "Action", 90, 5, 95, 20);
	openBtn->SetListener(-1, this, 0);
}

void CallbackWidget::SetData(void* set_owner, void* set_data)
{
	owner = set_owner;
	callback = (Callback)set_data;
}

void CallbackWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	callback(owner);
}