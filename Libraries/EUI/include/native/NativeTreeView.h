#pragma once

#include "NativeInclude.h"

class NativeTreeView : public WidgetImpl
{
public:

	NativeTreeView(EUIWidget* owner) : WidgetImpl(owner) {};

	virtual void  AddImage(const char* name) = 0;
	virtual void  DeleteItem(void* item) = 0;
	virtual void  ClearTree() = 0;
	virtual void* AddItem(const char* txt, int image, void* ptr, void* parent, int child_index, bool can_have_childs, const char* tooltip) = 0;
	virtual void  SetABSortChilds(void* item, bool sort) = 0;
	virtual void  SetItemText(void* item, const char* text) = 0;
	virtual void* GetSelectedItem() = 0;
	virtual void  SelectItem(void* item) = 0;
	virtual void  SetItemImage(void* item, int image) = 0;
	virtual void  GetItemText(void* item, std::string& text) = 0;
	virtual bool  IsItemOpened(void* item) = 0;
	virtual void  SetItemOpen(void* item, bool opened) = 0;
	virtual void* GetItemPtr(void* item) = 0;
	virtual void* GetItemParent(void* item) = 0;
	virtual int   GetItemChildCount(void* item) = 0;
	virtual void* GetItemChild(void* item, int index) = 0;
};
