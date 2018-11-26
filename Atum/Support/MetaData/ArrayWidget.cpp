
#include "ArrayWidget.h"
#include "MetaData.h"

void ArrayWidget::Init(EUICategories* parent, const char* catName, const char* labelName)
{
	ProperyWidget::Init(parent, catName, labelName);
	panel->SetListener(-1, this, EUIWidget::OnUpdate);

	label = new EUILabel(panel, labelName, 5, 5, 85, 20);
	label->Show(false);

	label = new EUILabel(panel, labelName, 5, 5, 85, 20);

	addBtn = new EUIButton(panel, "Add", 90, 5, 95, 20);
	addBtn->SetListener(-1, this, 0);

	elements = new EUICategories(parent, 10, 30, 190, -1);
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

	//int cur_size = adapter->GetSize();

	//pre_sel_item = -1;
	//cat->Show(false);

	if (adapter->sel_item)
	{
		sel_item = adapter->sel_item;
		addBtn->Enable(false);

		pre_sel_item = -1;

		if (elem_cats.size() == 0)
		{
			ElemCat elem;

			elem.cat = new EUICategories(elements, 10, 0, 190, -1);
			elements->RegisterChildInCategory("Selected Element", elem.cat);
			elem_cats.push_back(elem);
		}
	}
	else
	{
		int cur_size = adapter->GetSize();
		int cat_size = (int)elem_cats.size();

		if (cat_size < cur_size)
		{
			elem_cats.resize(cur_size);

			for (int i = cat_size; i < cur_size; i++)
			{
				elem_cats[i].btnDel = new EUIButton(elements, "Delete", 10, 0, 170, 25);
				elem_cats[i].btnDel->SetListener(i, this, 0);

				elem_cats[i].cat = new EUICategories(elements, 10, 30, 190, -1);

				char str[64];
				sprintf(str, "Element %i", i + 1);
				elements->RegisterChildInCategory(str, elem_cats[i].btnDel);
				elements->RegisterChildInCategory(str, elem_cats[i].cat);
			}
		}
		else
		{
			for (int i = cur_size; i < cat_size; i++)
			{
				elem_cats[i].btnDel->Show(false);
				elem_cats[i].cat->Show(false);
			}
		}

		for (int i = 0; i < cur_size; i++)
		{
			elem_cats[i].cat->Show(true);
			adapter->GetMetaData()->Prepare(adapter->GetItem(i));
			adapter->GetMetaData()->PrepareWidgets(elem_cats[i].cat);
		}
	}
}

void ArrayWidget::OnUpdate(EUIWidget* sender)
{
	MetaData::ArrayAdapter* adapter = (MetaData::ArrayAdapter*)prop;

	if (sel_item && *(sel_item) != pre_sel_item)
	{
		pre_sel_item = *(sel_item);

		EUICategories* cat = elem_cats[0].cat;

		if (pre_sel_item != -1)
		{
			cat->Show(true);
			adapter->GetMetaData()->Prepare(adapter->GetItem(pre_sel_item), root);
			adapter->GetMetaData()->PrepareWidgets(cat);
		}
		else
		{
			cat->Show(false);
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
		if (sender->GetID() != -1)
		{
			adapter->Delete(sender->GetID());

			SetData(nullptr);
		}
	}
}