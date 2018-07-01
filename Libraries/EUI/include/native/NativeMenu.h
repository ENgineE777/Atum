
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeMenu : public WinWidget
{
public:

	NativeMenu(EUIWidget* owner) : WinWidget(owner) {};

	virtual void AttachToWidget(EUIWidget* widget) = 0;
	virtual void ShowAsPopup(EUIWidget* parent, int x, int y) = 0;
	virtual void StartMenu(bool is_popup) = 0;
	virtual void AddItem(int id, const char* name) = 0;
	virtual void AddSeparator() = 0;
	virtual void StartSubMenu(const char* name) = 0;
	virtual void EndSubMenu() = 0;
};
