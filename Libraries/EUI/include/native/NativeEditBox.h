
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeEditBox : public WinWidget
{
public:

	NativeEditBox(EUIWidget* owner) : WinWidget(owner) {};

	virtual const char* GetText() = 0;
};
