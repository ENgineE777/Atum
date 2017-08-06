
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeLabel : public WinWidget
{
public:

	NativeLabel(EUIWidget* owner) : WinWidget(owner) {};
};
