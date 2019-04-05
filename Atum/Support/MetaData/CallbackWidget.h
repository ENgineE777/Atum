
#pragma once

#include "ProperyWidget.h"

class CallbackWidget : public ProperyWidget, public EUIWidget::Listener
{
public:

	typedef void(*Callback)(void* owner);

	EUIButton* openBtn;

	Callback callback;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void SetData(void* owner, void* set_data) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};