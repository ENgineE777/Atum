
#pragma once

#include "EUIWidget.h"
#include "Native/NativeMenu.h"

class EUIMenu : public EUIWidget
{
	friend class WinMenu;

	NativeMenu* Native();

public:

	EUIMenu(EUIWidget* parent);
	virtual ~EUIMenu();

	void AddItem(int id, const char* name);
	void AddSeparator();
	void StartSubMenu(const char* name);
	void EndSubMenu();
};
