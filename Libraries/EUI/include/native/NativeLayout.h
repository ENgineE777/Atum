
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeLayout : public WinWidget
{
public:

	NativeLayout(EUIWidget* owner) : WinWidget(owner) {};
};
