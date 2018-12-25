
#pragma once

#include "ProperyWidget.h"

class CallbackWidget : public ProperyWidget, public EUIWidget::Listener
{
public:

	typedef void(*Callback)(void* owner);

	EUIButton* openBtn;

	void* owner;
	Callback callback;

	void Init(EUICategories* parent, const char* catName, const char* labelName) override;
	void Prepare(void* owner, Callback callback);
	void SetData(void* set_data) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
};