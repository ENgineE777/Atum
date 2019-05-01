
#include <angelscript.h>
#include "MetaData.h"
#include "SceneObjects/2D/Sprite.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Scene/Scene.h"
#include "Services/Core/Core.h"

Scene* MetaData::scene = nullptr;

void MetaData::Prepare(void* set_owner, void* set_root)
{
	if (!inited)
	{
		Init();
		inited = true;
	}

	owner = set_owner;
	root = set_root ? set_root : owner;

	for (int i = 0; i < properties.size(); i++)
	{
		Property& prop = properties[i];
		prop.value = (uint8_t*)owner + prop.offset;

#ifdef EDITOR
		if (prop.type == Type::Callback)
		{
			prop.value = (uint8_t*)prop.callback;
		}
#endif

		if (prop.adapter)
		{
			prop.adapter->value = prop.value;

			if (prop.adapter->sel_item_offset != -1)
			{
				prop.adapter->sel_item = (int32_t*)((uint8_t*)root + prop.adapter->sel_item_offset);
			}
		}
	}
}

void MetaData::SetDefValues()
{
	for (int i = 0; i < properties.size(); i++)
	{
		Property& prop = properties[i];

		if (prop.type == Type::Boolean)
		{
			memcpy(prop.value, &prop.defvalue.boolean, sizeof(bool));
		}
		else
		if (prop.type == Type::Integer)
		{
			memcpy(prop.value, &prop.defvalue.integer, sizeof(int));
		}
		else
		if (prop.type == Type::Float)
		{
			memcpy(prop.value, &prop.defvalue.flt, sizeof(float));
		}
		else
		if (prop.type == Type::String || prop.type == Type::FileName)
		{
			*((std::string*)prop.value) = defStrings[prop.defvalue.string];
		}
		else
		if (prop.type == Type::Color)
		{
			memcpy(prop.value, prop.defvalue.color, sizeof(float) * 4);
		}
		else
		if (prop.type == Type::Enum)
		{
			MetaDataEnum& enm = enums[prop.defvalue.enumIndex];

			int value = enm.values[enm.defIndex];
			memcpy(prop.value, &value, sizeof(int));
		}
		else
		if (prop.type == Type::EnumString)
		{
			*((std::string*)prop.value) = "";
		}
	}
}

void MetaData::Load(JSONReader& reader)
{
	for (int i = 0; i < properties.size(); i++)
	{
		Property& prop = properties[i];

		if (prop.type == Type::Boolean)
		{
			bool val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(bool));
			}
		}
		else
		if (prop.type == Type::Integer || prop.type == Type::Enum)
		{
			int val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(int));
			}
		}
		else
		if (prop.type == Type::Float)
		{
			float val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(float));
			}
		}
		else
		if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
		{
			reader.Read(prop.propName.c_str(), *((std::string*)prop.value));
		}
		else
		if (prop.type == Type::Color)
		{
			reader.Read(prop.propName.c_str(), *((::Color*)prop.value));
		}
		else
		if (prop.type == Type::Array)
		{
			if (reader.EnterBlock(prop.propName.c_str()))
			{
				int count = 0;
				if (reader.Read("count", count))
				{
					prop.adapter->Resize(count);
				
					for (int i = 0; i < count; i++)
					{
						reader.EnterBlock("Elem");

						prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
						prop.adapter->GetMetaData()->Load(reader);

						reader.LeaveBlock();
					}
				}

				reader.LeaveBlock();
			}
		}
		else
		if (prop.type == Type::SceneObject)
		{
			if (reader.EnterBlock(prop.propName.c_str()))
			{
				SceneObjectRef* ref = (SceneObjectRef*)prop.value;

				reader.Read("uid", ref->uid);
				reader.Read("child_uid", ref->child_uid);
				reader.Read("is_asset", ref->is_asset);

				reader.LeaveBlock();
			}
		}
		else
		if (prop.type == Type::Sprite)
		{
			Sprite::Load(reader, (Sprite::Data*)prop.value, prop.propName.c_str());
		}
	}
}

void MetaData::PostLoad()
{
	for (auto& prop : properties)
	{
		if (prop.type == Type::Array)
		{
			for (int i = 0; i < prop.adapter->GetSize(); i++)
			{
				prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
				prop.adapter->GetMetaData()->PostLoad();
			}
		}
		else
		if (prop.type == Type::SceneObject)
		{
			SceneObjectRef* ref = (SceneObjectRef*)prop.value;

			ref->object = scene->FindByUID(ref->uid, ref->child_uid, ref->is_asset);
		}
	}
}

void MetaData::Save(JSONWriter& writer)
{
	for (int i = 0; i < properties.size(); i++)
	{
		Property& prop = properties[i];

		if (prop.type == Type::Boolean)
		{
			writer.Write(prop.propName.c_str(), *((bool*)prop.value));
		}
		else
		if (prop.type == Type::Integer || prop.type == Type::Enum)
		{
			writer.Write(prop.propName.c_str(), *((int*)prop.value));
		}
		else
		if (prop.type == Type::Float)
		{
			writer.Write(prop.propName.c_str(), *((float*)prop.value));
		}
		else
		if (prop.type == String || prop.type == EnumString || prop.type == FileName)
		{
			writer.Write(prop.propName.c_str(), ((std::string*)prop.value)->c_str());
		}
		else
		if (prop.type == Type::Color)
		{
			writer.Write(prop.propName.c_str(), *((::Color*)prop.value));
		}
		else
		if (prop.type == Type::Array)
		{
			writer.StartBlock(prop.propName.c_str());

			int count = prop.adapter->GetSize();
			writer.Write("count", count);
			
			writer.StartArray("Elem");

			for (int i = 0; i < count; i++)
			{
				writer.StartBlock(nullptr);

				prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
				prop.adapter->GetMetaData()->Save(writer);

				writer.FinishBlock();
			}

			writer.FinishArray();

			writer.FinishBlock();
		}
		else
		if (prop.type == Type::SceneObject)
		{
			SceneObjectRef* ref = (SceneObjectRef*)prop.value;

			writer.StartBlock(prop.propName.c_str());

			writer.Write("uid", ref->uid);
			writer.Write("child_uid", ref->child_uid);
			writer.Write("is_asset", ref->is_asset);

			writer.FinishBlock();
		}
		else
		if (prop.type == Type::Sprite)
		{
			Sprite::Save(writer, (Sprite::Data*)prop.value, prop.propName.c_str());
		}
	}
}

void MetaData::Copy(void* source)
{
	for (auto& prop : properties)
	{
		uint8_t* src = (uint8_t*)source + prop.offset;

		if (prop.type == Type::Boolean)
		{
			memcpy(prop.value, src, sizeof(bool));
		}
		else
		if (prop.type == Type::Integer || prop.type == Type::Enum)
		{
			memcpy(prop.value, src, sizeof(int));
		}
		else
		if (prop.type == Type::Float)
		{
			memcpy(prop.value, src, sizeof(float));
		}
		else
		if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
		{
			*((std::string*)prop.value) = *((std::string*)src);
		}
		else
		if (prop.type == Type::Color)
		{
			memcpy(prop.value, src, sizeof(float) * 4);
		}
		else
		if (prop.type == Type::Array)
		{
			prop.adapter->value = src;
			int count = prop.adapter->GetSize();
			
			prop.adapter->value = prop.value;
			prop.adapter->Resize(count);

			for (int i = 0; i < count; i++)
			{
				prop.adapter->value = src;
				uint8_t* src_item = prop.adapter->GetItem(i);

				prop.adapter->value = prop.value;

				prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
				prop.adapter->GetMetaData()->Copy(src_item);
			}
		}
		else
		if (prop.type == Type::Sprite)
		{
			Sprite::Copy((Sprite::Data*)src, (Sprite::Data*)prop.value);
		}
	}
}

void MetaData::BindToScript( const char* script_class_name)
{
	if (!inited)
	{
		Init();
		inited = true;
	}

	for (auto& prop : properties)
	{
		char decl[128];

		decl[0] = 0;

		if (prop.type == Boolean)
		{
			StringUtils::Printf(decl, 128, "bool %s", prop.propName.c_str());
		}
		else
		if (prop.type == Integer || prop.type == Enum)
		{
			StringUtils::Printf(decl, 128, "int %s", prop.propName.c_str());
		}
		else
		if (prop.type == Float)
		{
			StringUtils::Printf(decl, 128, "float %s", prop.propName.c_str());
		}

		if (decl[0])
		{
			core.scripts.RegisterObjectProperty(script_class_name, decl, (int)prop.offset);
		}
		
	}
}

#ifdef EDITOR
void MetaData::PrepareWidgets(EUICategories* parent)
{
	for (auto& prop : properties)
	{
		ProperyWidget* widget = nullptr;

		if (prop.widgets.count(parent) > 0)
		{
			widget = prop.widgets[parent];
		}
		else
		{
			if (prop.type == Type::Boolean)
			{
				widget = new BoolWidget();
			}
			else
			if (prop.type == Type::Integer)
			{
				widget = new IntWidget();
			}
			else
			if (prop.type == Type::Float)
			{
				widget = new FloatWidget();
			}
			else
			if (prop.type == Type::String)
			{
				widget = new StringWidget();
			}
			else
			if (prop.type == Type::FileName)
			{
				widget = new FileNameWidget();
			}
			else
			if (prop.type == Type::Color)
			{
				widget = new ColorWidget();
			}
			else
			if (prop.type == Type::Enum)
			{
				widget = new EnumWidget(&enums[prop.defvalue.enumIndex]);
			}
			else
			if (prop.type == Type::EnumString)
			{
				widget = new EnumStringWidget(prop.enum_callback);
			}
			else
			if (prop.type == Type::Callback)
			{
				widget = new CallbackWidget();
			}
			else
			if (prop.type == Type::Array)
			{
				widget = new ArrayWidget();
				((ArrayWidget*)widget)->prop = prop.adapter;
				((ArrayWidget*)widget)->root = root;
			}
			else
			if (prop.type == Type::Sprite)
			{
				widget = new SpriteWidget();
			}
			else
			if (prop.type == Type::SceneObject)
			{
				widget = new SceneObjectWidget();
			}

			widget->Init(parent, prop.catName.c_str(), prop.propName.c_str());
			prop.widgets[parent] = widget;
		}

		widget->SetData(owner, prop.value);

		widget->Show(true);
	}
}

void MetaData::UpdateWidgets()
{
	for (auto& prop : properties)
	{
		for (auto& widget : prop.widgets)
		{
			if (!widget.second->panel->IsVisible())
			{
				continue;
			}

			widget.second->SetData(owner, prop.value);
		}
	}
}

bool MetaData::IsValueWasChanged()
{
	bool res = false;
	for (auto& prop : properties)
	{
		if (prop.type == Type::Array)
		{
			res |= prop.adapter->GetMetaData()->IsValueWasChanged();
			continue;
		}

		for (auto& widget : prop.widgets)
		{
			if (!widget.second->panel->IsVisible())
			{
				continue;
			}

			res |= widget.second->changed;
			widget.second->changed = false;
		}
	}

	return res;
}

void MetaData::HideWidgets()
{
	for (auto& prop : properties)
	{
		for (auto& widget : prop.widgets)
		{
			widget.second->Show(false);
		}
	}
}
#endif