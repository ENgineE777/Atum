
#pragma once

#include "NativeInclude.h"

class NativeWindow : public WidgetImpl
{
public:

	NativeWindow(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void Maximaze() = 0;
	virtual void SetAtScreenCenter() = 0;
	virtual void Close() = 0;
};
