
#pragma once

#include "EUIWidget.h"
#include "Native/NativeCategories.h"

class EUICategories : public EUIWidget
{
	friend class WinCategories;

	struct Category
	{
		bool  opened;
		bool  visible;
		char  name[128];
		std::vector<EUIWidget*> childs;
		std::vector<bool> childsVis;
		float y;

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

	NativeCategories* Native();

public:

	EUICategories(EUIWidget* parent, float x, float y, float w, float h);
	virtual ~EUICategories();

	virtual void OnChildShow(int index, bool set);
	virtual void RegisterChildInCategory(const char* name, EUIWidget* widget);
};
