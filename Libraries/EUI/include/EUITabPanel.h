
#pragma once

#include "EUIWidget.h"
#include "Native/NativeTabPanel.h"

class EUITabSheet;

class EUITabPanel : public EUIWidget
{
	friend class WinTabPanel;

	NativeTabPanel* Native();

public:

	EUITabPanel(EUIWidget* parent, float x, float y, float w, float h);
	virtual ~EUITabPanel();

	virtual EUITabSheet* AddTab(const char* txt);
	virtual EUITabSheet* GetTab(int index);
	virtual int  GetCurrentTabIndex();
	virtual void DeleteTab(int index);
	virtual void ClearTabs();
};
