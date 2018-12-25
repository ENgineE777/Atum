
#include "CallbackWidget.h"

void CallbackWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	openBtn = new EUIButton(panel, "Edit", 90, 5, 95, 20);
	openBtn->SetListener(-1, this, 0);
}

void CallbackWidget::Prepare(void* set_owner, Callback set_callback)
{
	owner = set_owner;
	callback = set_callback;
}

void CallbackWidget::SetData(void* set_data)
{
}

void CallbackWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	callback(owner);
}