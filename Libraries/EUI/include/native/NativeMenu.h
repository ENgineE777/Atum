
#pragma once

#include "NativeInclude.h"

class NativeMenu : public WidgetImpl
{
public:

	NativeMenu(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void AttachToWidget(EUIWidget* widget) = 0;
	virtual void ShowAsPopup(EUIWidget* parent, int x, int y) = 0;
	virtual void StartMenu(bool is_popup) = 0;
	virtual void AddItem(int id, const char* name, bool enabled) = 0;
	virtual void AddSeparator() = 0;
	virtual void StartSubMenu(const char* name) = 0;
	virtual void EndSubMenu() = 0;
};
