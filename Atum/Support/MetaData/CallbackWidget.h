
#pragma once

#include "ProperyWidget.h"

class CallbackWidget : public ProperyWidget
{
public:

	typedef void(*Callback)(void* owner);

	EUIButton* openBtn;

	class Listiner : public EUIWidget::Listener
	{
	public:
		CallbackWidget* owner;

		virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	};

	void* owner;
	Callback callback;
	Listiner listiner;

	virtual void Init(EUICategories* parent, const char* catName, const char* labelName);
	virtual void Prepare(void* owner, Callback callback);
	virtual void SetData(void* set_data);
};