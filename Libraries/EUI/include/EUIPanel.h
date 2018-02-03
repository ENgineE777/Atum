
#pragma once

#include "EUIWidget.h"
#include "Native/NativePanel.h"

class EUIPanel : public EUIWidget
{
	friend class WinPanel;

	NativePanel* Native();

public:

	EUIPanel(EUIWidget* parent, int x, int y, int w, int h);
	virtual ~EUIPanel();
};
