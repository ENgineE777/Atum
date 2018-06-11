
#include "UIWidgetAsset.h"
#include "Editor/Gizmo.h"

CLASSFACTORYDECL(UIWidgetAsset)

UIWidgetAsset::~UIWidgetAsset()
{
}

void UIWidgetAsset::Init()
{
}

void UIWidgetAsset::ApplyProperties()
{
	for (auto child : childs)
	{
		child->ApplyProperties();
	}
}

void UIWidgetAsset::Load(JSONReader& reader)
{
	if (source)
	{
		source->GetMetaData()->Prepare(this);
		source->GetMetaData()->Copy(source);
	}

	SceneAsset::Load(reader);
	
	if (source_is_asset)
	{
		char source_name[512];
		if (reader.Read("source", source_name, 512))
		{
			SetSource((UIWidgetAsset*)owner->FindAsset(source_name), false);
		}
	}

	int index = 0;
	UIWidgetAsset* source_child = nullptr;

	while (reader.EnterBlock("Childs"))
	{
		if (source)
		{
			source_child = source->childs[index];
		}

		char type[512];
		reader.Read("type", type, 512);

		UIWidgetAsset* obj = ClassFactoryUIWidgetAsset::Create(type);

		if (obj)
		{
			obj->owner = owner;
			obj->SetSource(source_child, false);
			obj->className = type;
			reader.Read("name", type, 512);

			obj->SetName(type);
			obj->Init();

			AddChild(obj);
			obj->Load(reader);
			obj->ApplyProperties();
		}

		reader.LeaveBlock();

		index++;
	}
}

void UIWidgetAsset::Save(JSONWriter& writer)
{
	SceneAsset::Save(writer);

	if (source && source_is_asset)
	{
		writer.Write("source", source->GetName());
	}

	writer.StartArray("Childs");

	int index = 0;
	UIWidgetAsset* source_child = nullptr;

	for (auto child : childs)
	{
		writer.StartBlock(nullptr);

		writer.Write("type", child->GetClassName());
		writer.Write("name", child->GetName());

		child->Save(writer);

		writer.FinishBlock();

		index++;
	}

	writer.FinishArray();
}

void UIWidgetAsset::CalcState()
{
	cur_color = parent->color * color;

	trans.offset = 0.0f;
	trans.mat_parent = parent->trans.mat_global;

	if (horzAlign == align_center)
	{
		trans.mat_parent.Pos().x += parent->trans.size.x * 0.5f;
	}
	else
	if (horzAlign == align_right)
	{
		trans.mat_parent.Pos().x += parent->trans.size.x;
	}

	if (vertAlign == align_center)
	{
		trans.mat_parent.Pos().y += parent->trans.size.y * 0.5f;
	}
	else
	if (vertAlign == align_bottom)
	{
		trans.mat_parent.Pos().y += parent->trans.size.y;
	}

	trans.axis.x = (horzAlign == align_right) ? -1.0f : 1.0f;
	trans.axis.y = (vertAlign == align_bottom) ? -1.0f : 1.0f;

	trans.BuildMatrices();
}

void UIWidgetAsset::Draw(float dt)
{

}

void UIWidgetAsset::AddChild(UIWidgetAsset* node, int index)
{
	node->parent = this;

	if (index == -1)
	{
		childs.push_back(node);
	}
	else
	{
		childs.insert(childs.begin() + index, node);
	}
}

void UIWidgetAsset::DeleteChild(UIWidgetAsset* node)
{
	int index = 0;
	for (auto child : childs)
	{
		if (child == node)
		{
			childs.erase(childs.begin() + index);
			break;
		}

		index++;
	}
}

void UIWidgetAsset::SetSource(UIWidgetAsset* set_source, bool remove_from_prev)
{
#ifdef EDITOR
	if (source && remove_from_prev)
	{
		int index = 0;
		for (auto inst : source->instances)
		{
			if (inst == this)
			{
				source->instances.erase(source->instances.begin() + index);
				break;
			}

			index++;
		}
	}
#endif

	source = set_source;

#ifdef EDITOR
	if (source)
	{
		source->instances.push_back(this);
	}
#endif
}

void UIWidgetAsset::DeleteChilds()
{
	for (auto child : childs)
	{
		child->SetSource(nullptr, true);
		child->DeleteChilds();
		child->Release();
	}

	childs.clear();
}

#ifdef EDITOR
void UIWidgetAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);
}

void UIWidgetAsset::StoreProperties()
{

}

void UIWidgetAsset::RestoreProperties()
{

}

#endif