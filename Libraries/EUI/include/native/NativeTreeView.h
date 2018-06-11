#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeTreeView : public WinWidget
{
public:

	NativeTreeView(EUIWidget* owner) : WinWidget(owner) {};

	virtual void  AddImage(const char* name) = 0;
	virtual void  DeleteItem(void* item) = 0;
	virtual void  ClearTree() = 0;
	virtual void  SortItems(void* root, bool recursive) = 0;
	virtual void* AddItem(const char* txt, int image, void* ptr, void* parent, int child_index, bool can_have_childs, const char* tooltip) = 0;
	virtual void  SetItemText(void* item, const char* text) = 0;
	virtual void* GetSelectedItem() = 0;
	virtual void  SelectItem(void* item) = 0;
	virtual void* GetItemPtr(void* item) = 0;
	virtual void  MoveDraggedItem() = 0;
	virtual void  StartPopupMenu() = 0;
	virtual void  PopupMenuAddItem(int id, const char* name) = 0;
	virtual void  PopupMenuAddSeparator() = 0;
	virtual void  PopupMenuStartSubMenu(const char* name) = 0;
	virtual void  PopupMenuEndSubMenu() = 0;
};
