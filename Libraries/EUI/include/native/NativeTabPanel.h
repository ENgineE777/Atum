
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeTabPanel : public WinWidget
{
public:

	NativeTabPanel(EUIWidget* owner) : WinWidget(owner) {};

	virtual int  GetCurrentTabIndex() = 0;
	virtual void DeleteTab(int index) = 0;
	virtual void ClearTabs() = 0;
};
