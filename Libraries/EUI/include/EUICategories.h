
#pragma once

#include "EUIWidget.h"
#include "Native/NativeCategories.h"

class EUICategories : public EUIWidget
{
	friend class WinCategories;
	friend class WinDX11Categories;

	struct Category
	{
		bool  opened;
		bool  visible;
		char  name[128];
		std::vector<EUIWidget*> childs;
		std::vector<bool> childsVis;
		int y;

		Category()
		{
			opened = false;
			visible = false;
			name[0] = 0;
			y = 0;
		};
	};

	bool allowCallOnChildShow;
	std::vector<Category> categories;

	bool auto_size = false;
	NativeCategories* Native();
	void AddChild(EUIWidget* child) override;
	void DelChild(EUIWidget* child) override;

public:

	EUICategories(EUIWidget* parent, int x, int y, int w, int h);
	virtual ~EUICategories();

	void DeleteChilds() override;
	void OnChildShow(int index, bool set) override;
	virtual void RegisterChildInCategory(const char* name, EUIWidget* widget, float abc_sort);
};
