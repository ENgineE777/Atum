
#pragma once

#include "NativeInclude.h"

class NativeButton : public WidgetImpl
{
public:

	NativeButton(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void SetImage(int img, const char* image_name) = 0;
};
