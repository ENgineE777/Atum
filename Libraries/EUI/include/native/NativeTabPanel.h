
#pragma once

#include "NativeInclude.h"

class NativeTabPanel : public WidgetImpl
{
public:

	NativeTabPanel(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual int  GetCurrentTabIndex() = 0;
	virtual void DeleteTab(int index) = 0;
	virtual void ClearTabs() = 0;
};
