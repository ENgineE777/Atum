
#pragma once

#include "EUIWidget.h"
#include "Native/NativeMenu.h"

class EUIMenu : public EUIWidget
{
	friend class WinMenu;

	NativeMenu* Native();

public:

	EUIMenu();
	virtual ~EUIMenu();

	void AttachToWidget(EUIWidget* widget);
	void ShowAsPopup(EUIWidget* parent, int x, int y);

	void StartMenu(bool is_popup);
	void AddItem(int id, const char* name, bool enabled = true);
	void AddSeparator();
	void StartSubMenu(const char* name);
	void EndSubMenu();
};
