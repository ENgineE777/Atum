
#pragma once

#include "NativeInclude.h"

class NativePanel : public WidgetImpl
{
public:

	NativePanel(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void SetTexture(void* texture) {};
};
