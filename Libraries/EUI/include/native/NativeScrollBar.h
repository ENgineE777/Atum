
#pragma once

#include "NativeInclude.h"

class NativeScrollBar : public WidgetImpl
{
public:

	NativeScrollBar(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void SetPosition(int pos) = 0;
	virtual void SetLimit() = 0;
};