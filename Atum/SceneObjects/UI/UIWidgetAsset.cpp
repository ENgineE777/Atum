
#include "UIWidgetAsset.h"

UIWidgetAsset::~UIWidgetAsset()
{
}

void UIWidgetAsset::Init()
{
}

void UIWidgetAsset::Release()
{
	SetSource(nullptr, true);

	SceneAsset::Release();
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
	
	char source_name[512];

	if (source_is_asset)
	{
		if (reader.Read("source", source_name, 512))
		{
			SetSource((UIWidgetAsset*)owner->FindByName(source_name, true), false);
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

		auto decl = ClassFactoryUIWidgetAsset::Find(type);
		UIWidgetAsset* obj = decl->Create(type);

		if (obj)
		{
			obj->owner = owner;
			obj->class_name = decl->GetName();
			obj->SetSource(source_child, false);
			obj->script_class_name = decl->GetShortName();

			obj->Init();

			AddChild(obj);

			reader.Read("uid", obj->uid);

			if (obj->uid == 0)
			{
				owner->GenerateChildUID(obj);
			}

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

		writer.Write("type", child->class_name);
		writer.Write("uid", child->uid);

		child->Save(writer);

		writer.FinishBlock();

		index++;
	}

	writer.FinishArray();
}

void UIWidgetAsset::CalcState()
{
	Vector2 parent_size;

	if (parent)
	{
		parent_size = parent->trans.size;
		cur_color = parent->color * color;
	}
	else
	{
		parent_size.x = core.render.GetDevice()->GetWidth() * 1024.0f / core.render.GetDevice()->GetHeight();
		parent_size.y = 1024.0f;

		cur_color = color;
	}

	if (horzSize == fill_parent)
	{
		trans.pos.x = 0;
		trans.size.x = parent_size.x - right_padding.x - left_padding.x;
	}

	if (vertSize == fill_parent)
	{
		trans.pos.y = 0;
		trans.size.y = parent_size.y - right_padding.y - left_padding.y;
	}

	if (parent)
	{
		trans.mat_parent = parent->trans.mat_global;
		Vector2 offset = parent->trans.size * parent->trans.offset;
		trans.mat_parent.Pos().x -= offset.x;
		trans.mat_parent.Pos().y -= offset.y;
	}
	else
	{
		trans.mat_parent = Matrix();
	}

	if (horzAlign == align_left)
	{
		trans.mat_parent.Pos().x += left_padding.x;
	}
	else
	if (horzAlign == align_center)
	{
		trans.mat_parent.Pos().x += left_padding.x + (parent_size.x - right_padding.x - left_padding.x) * 0.5f;
	}
	else
	if (horzAlign == align_right)
	{
		trans.mat_parent.Pos().x += -right_padding.x + parent_size.x;
	}

	if (vertAlign == align_top)
	{
		trans.mat_parent.Pos().y += left_padding.y;
	}
	else
	if (vertAlign == align_center)
	{
		trans.mat_parent.Pos().y += left_padding.y + (parent_size.y - right_padding.x - left_padding.x) * 0.5f;
	}
	else
	if (vertAlign == align_bottom)
	{
		trans.mat_parent.Pos().y += (-right_padding.y) + parent_size.y;
	}

	trans.axis.x = (horzAlign == align_right) ? -1.0f : 1.0f;
	trans.axis.y = (vertAlign == align_bottom) ? -1.0f : 1.0f;
	trans.depth = 0.2f;

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
			if (inst.GetObject() == this)
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
		if (remove_from_prev || (!remove_from_prev && !StringUtils::IsEqual(class_name, "UIViewInstanceAsset") && !StringUtils::IsEqual(class_name, "UIViewInstance")))
		{
			source->instances.push_back(SceneAsset::AssetInstance(this));
		}
	}
#endif
}

uint32_t UIWidgetAsset::GetParentUID()
{
	if (parent)
	{
		return parent->GetParentUID();
	}

	return GetUID();
}

SceneObject* UIWidgetAsset::GetChild(uint32_t uid)
{
	for (auto child : childs)
	{
		if (child->GetUID() == uid)
		{
			return child;
		}

		SceneObject* res = child->GetChild(uid);

		if (res)
		{
			return res;
		}
	}

	return nullptr;
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
void UIWidgetAsset::SetOwner(Scene* owner)
{
	SceneObject::SetOwner(owner);

	for (auto& child : childs)
	{
		child->SetOwner(owner);
	}
}

bool UIWidgetAsset::UsingCamera2DPos()
{
	return !AddedToTreeByParent();
}

void UIWidgetAsset::SetEditMode(bool ed)
{
	SceneAsset::SetEditMode(ed);
	Gizmo::inst->SetTrans2D(ed ? &trans : nullptr, !AddedToTreeByParent() ? 0xffff : 0, true);

	if (ed)
	{
		Gizmo::inst->SetTrans2DWidgets(GetMetaData()->GetFloatEditBox("x"), GetMetaData()->GetFloatEditBox("y"),
		                               GetMetaData()->GetFloatEditBox("width"), GetMetaData()->GetFloatEditBox("height"),
		                               GetMetaData()->GetFloatEditBox("anchorn_x"), GetMetaData()->GetFloatEditBox("anchorn_y"));
	}
}

void UIWidgetAsset::StoreProperties()
{

}

void UIWidgetAsset::RestoreProperties()
{

}

#endif