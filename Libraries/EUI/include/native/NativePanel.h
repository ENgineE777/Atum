
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativePanel : public WinWidget
{
public:

	NativePanel(EUIWidget* owner) : WinWidget(owner) {};
};
