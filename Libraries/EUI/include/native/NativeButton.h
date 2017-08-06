
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeButton : public WinWidget
{
public:

	NativeButton(EUIWidget* owner) : WinWidget(owner) {};

	virtual void SetImage(int img, const char* image_name) = 0;
};
