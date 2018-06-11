#pragma once

#include "EUIWidget.h"
#include "Native/NativeTreeView.h"

class EUITreeView : public EUIWidget
{
	friend class WinTreeView;

	NativeTreeView* Native();

public:

	EUITreeView(EUIWidget* parent, int x, int y, int w, int h);
	virtual ~EUITreeView();

	virtual void  AddImage(const char* name);
	virtual void  DeleteItem(void* item);
	virtual void  ClearTree();
	virtual void  SortItems(void* root, bool recursive);
	virtual void* AddItem(const char* txt, int image, void* ptr, void* parent, int child_index, bool can_have_childs, const char* tooltip = nullptr);
	virtual void  SetItemText(void* item, const char* text);
	virtual void* GetSelectedItem();
	virtual void  SelectItem(void* item);
	virtual void* GetItemPtr(void* item);
	virtual void  MoveDraggedItem();
	virtual void  StartPopupMenu();
	virtual void  PopupMenuAddItem(int id, const char* name);
	virtual void  PopupMenuAddSeparator();
	virtual void  PopupMenuStartSubMenu(const char* name);
	virtual void  PopupMenuEndSubMenu();
};

