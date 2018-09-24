
#include "ArrayWidget.h"
#include "MetaData.h"

void ArrayWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);

	label = new EUILabel(panel, labelName, 5, 5, 85, 20);

	addBtn = new EUIButton(panel, "Add", 90, 5, 95, 20);
	addBtn->SetListener(-1, this, 0);

	elements = new EUICategories(parent, 10, 30, 190, 500);
	parent->RegisterChildInCategory(catName, elements);
}

void ArrayWidget::Show(bool show)
{
	ProperyWidget::Show(show);
	elements->Show(show);
}

void ArrayWidget::SetData(void* set_data)
{
	MetaData::ArrayAdapter* adapter = (MetaData::ArrayAdapter*)prop;

	int cur_size = adapter->GetSize();
	int cat_size = (int)elem_cats.size();

	if (cat_size < cur_size)
	{
		elem_cats.resize(cur_size);

		for (int i = cat_size; i<cur_size; i++)
		{
			elem_cats[i].btnDel = new EUIButton(elements, "Delete", 10, 0, 170, 25);
			elem_cats[i].btnDel->SetListener(i, this, 0);

			if (i < 10)
			{
				elem_cats[i].cat = new EUICategories(elements, 10, 30, 190, 150);

				char str[64];
				sprintf(str, "Element %i", i + 1);
				elements->RegisterChildInCategory(str, elem_cats[i].btnDel);
				elements->RegisterChildInCategory(str, elem_cats[i].cat);
			}
		}
	}
	else
	{
		for (int i = cur_size; i<cat_size;i++)
		{
			if (i < 10)
			{
				elem_cats[i].btnDel->Show(false);
				elem_cats[i].cat->Show(false);
			}
		}
	}

	for (int i = 0; i < cur_size; i++)
	{
		if (i < 10)
		{
			elem_cats[i].cat->Show(true);
			adapter->GetMetaData()->Prepare(adapter->GetItem(i));
			adapter->GetMetaData()->PrepareWidgets(elem_cats[i].cat);
		}
	}
}

void ArrayWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	MetaData::ArrayAdapter* adapter = (MetaData::ArrayAdapter*)prop;

	if (sender == addBtn)
	{
		adapter->PushBack();
		adapter->GetMetaData()->Prepare(adapter->GetItem((int)adapter->GetSize() - 1));
		adapter->GetMetaData()->SetDefValues();

		SetData(nullptr);
	}
	else
	{
		adapter->Delete(sender->GetID());

		SetData(nullptr);
	}
}