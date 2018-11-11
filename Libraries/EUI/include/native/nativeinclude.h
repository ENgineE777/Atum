
#pragma once

#if defined(PLATFORM_WIN)
#include "native/win/WinWidget.h"
#define WidgetImpl WinWidget
#elif defined(PLATFORM_WIN_DX11)
#include "native/win_dx11/WinDX11Widget.h"
#define WidgetImpl WinDX11Widget
#else
#include "NativeWidget.h"
#define WidgetImpl NativeWidget
#endif