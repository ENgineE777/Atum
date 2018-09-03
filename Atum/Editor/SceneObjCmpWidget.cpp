
#include "SceneObjCmpWidget.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Scene/SceneAsset.h"

void SceneObjCmpWidget::Init(EUICategories* parent)
{
	panel = new EUIPanel(parent, 0, 0, 200, 135);
	cbox = new EUIComboBox(panel, 5, 5, 160, 20);
	cbox->SetListener(-1, this, 0);
	cbox->AddItem("123");
	cbox->AddItem("abc");

	delBtn = new EUIButton(panel, "Del", 170, 5, 25, 20);
	delBtn->SetListener(-1, this, 0);

	lbox = new EUIListBox(panel, 5, 30, 190, 100, true);
	lbox->SetListener(-1, this, 0);

	parent->RegisterChildInCategory("Components", panel);

	Show(false);
}

void SceneObjCmpWidget::Prepare(SceneObject* set_obj)
{
	obj = set_obj;

	if (!obj)
	{
		Show(false);
		return;
	}

	cbox->ClearList();
	bool added = false;

	for (const auto& decl : ClassFactorySceneObjectComp::Decls())
	{
		if (decl->only_for_instances)
		{
			continue;
		}

		if (decl->only_for_assets && !obj->IsAsset())
		{
			continue;
		}

		if (decl->IsApplicable(obj))
		{
			added = true;
			cbox->AddItem(decl->GetShortName());
		}
	}

	cbox->SetCurString(-1);

	lbox->ClearList();

	for (auto comp : obj->components)
	{
		lbox->AddItem(ClassFactorySceneObjectComp::Find(comp->class_name)->GetName(), comp);
	}

	Show(added);
}

void SceneObjCmpWidget::Show(bool show)
{
	panel->Show(show);
}

void SceneObjCmpWidget::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == delBtn)
	{
		SceneObjectComp* comp = (SceneObjectComp*)lbox->GetSelectedItemData();

		if (comp && !ClassFactorySceneObjectComp::Find(comp->class_name)->only_for_instances)
		{
			if (obj->IsAsset())
			{
				SceneAsset* asset = (SceneAsset*)obj;
				SceneAssetComp* asset_comp = (SceneAssetComp*)comp;

				for (auto inst : asset->instances)
				{
					for (int index = 0; index < inst->components.size(); index++)
					{
						if (inst->components[index]->class_name == asset_comp->inst_class_name)
						{
							inst->components.erase(inst->components.begin() + index);
							break;
						}
					}
				}
			}

			for (int index = 0; index < obj->components.size(); index++)
			{
				if (obj->components[index] == comp)
				{
					obj->components.erase(obj->components.begin() + index);
					break;
				}
			}

			lbox->DeleteItemByData(comp);
		}
	}
}

void SceneObjCmpWidget::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	if (index == -1)
	{
		return;
	}

	const auto& decl = ClassFactorySceneObjectComp::Find(cbox->GetCurString());

	bool added = false;

	for (auto& comp : obj->components)
	{
		if (decl->GetName() == obj->className)
		{
			added = true;
			break;
		}
	}

	if (!added)
	{
		SceneObjectComp* comp = decl->Create();

		comp->class_name = decl->GetName();
		comp->Init();

		obj->components.push_back(comp);

		if (obj->IsAsset())
		{
			SceneAsset* asset = (SceneAsset*)obj;
			SceneAssetComp* asset_comp = (SceneAssetComp*)comp;

			for (auto inst : asset->instances)
			{
				inst->components.push_back(asset_comp->CreateInstance());
			}
		}

		lbox->AddItem(decl->GetShortName(), comp);
	}

	cbox->SetCurString(-1);
}