
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeScrollBar : public WinWidget
{
public:

	NativeScrollBar(EUIWidget* owner) : WinWidget(owner) {};

	virtual void SetPosition(int pos) = 0;
	virtual void SetLimit() = 0;
};