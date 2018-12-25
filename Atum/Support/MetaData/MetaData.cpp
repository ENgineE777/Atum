
#include <angelscript.h>
#include "MetaData.h"
#include "SceneObjects/2D/Sprite.h"

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

		if (prop.type == Boolean)
		{
			memcpy(prop.value, &prop.defvalue.boolean, sizeof(bool));
		}
		else
		if (prop.type == Integer)
		{
			memcpy(prop.value, &prop.defvalue.integer, sizeof(int));
		}
		else
		if (prop.type == Float)
		{
			memcpy(prop.value, &prop.defvalue.flt, sizeof(float));
		}
		else
		if (prop.type == String || prop.type == FileName)
		{
			*((std::string*)prop.value) = defStrings[prop.defvalue.string];
		}
		else
		if (prop.type == Clor)
		{
			memcpy(prop.value, prop.defvalue.color, sizeof(float) * 4);
		}
		else
		if (prop.type == Enum)
		{
			MetaDataEnum& enm = enums[prop.defvalue.enumIndex];

			int value = enm.values[enm.defIndex];
			memcpy(prop.value, &value, sizeof(int));
		}
		else
		if (prop.type == EnumString)
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

		if (prop.type == Boolean)
		{
			bool val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(bool));
			}
		}
		else
		if (prop.type == Integer || prop.type == Enum)
		{
			int val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(int));
			}
		}
		else
		if (prop.type == Float)
		{
			float val;
			if (reader.Read(prop.propName.c_str(), val))
			{
				memcpy(prop.value, &val, sizeof(float));
			}
		}
		else
		if (prop.type == String || prop.type == EnumString || prop.type == FileName)
		{
			reader.Read(prop.propName.c_str(), *((std::string*)prop.value));
		}
		else
		if (prop.type == Clor)
		{
			reader.Read(prop.propName.c_str(), *((Color*)prop.value));
		}
		else
		if (prop.type == Array)
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
		if (prop.type == Sprite)
		{
			Sprite::Load(reader, (Sprite::Data*)prop.value, prop.propName.c_str());
		}
	}
}

void MetaData::Save(JSONWriter& writer)
{
	for (int i = 0; i < properties.size(); i++)
	{
		Property& prop = properties[i];

		if (prop.type == Boolean)
		{
			writer.Write(prop.propName.c_str(), *((bool*)prop.value));
		}
		else
		if (prop.type == Integer || prop.type == Enum)
		{
			writer.Write(prop.propName.c_str(), *((int*)prop.value));
		}
		else
		if (prop.type == Float)
		{
			writer.Write(prop.propName.c_str(), *((float*)prop.value));
		}
		else
		if (prop.type == String || prop.type == EnumString || prop.type == FileName)
		{
			writer.Write(prop.propName.c_str(), ((std::string*)prop.value)->c_str());
		}
		else
		if (prop.type == Clor)
		{
			writer.Write(prop.propName.c_str(), *((Color*)prop.value));
		}
		else
		if (prop.type == Array)
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
		if (prop.type == Sprite)
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

		if (prop.type == Boolean)
		{
			memcpy(prop.value, src, sizeof(bool));
		}
		else
		if (prop.type == Integer || prop.type == Enum)
		{
			memcpy(prop.value, src, sizeof(int));
		}
		else
		if (prop.type == Float)
		{
			memcpy(prop.value, src, sizeof(float));
		}
		else
		if (prop.type == String || prop.type == EnumString || prop.type == FileName)
		{
			*((std::string*)prop.value) = *((std::string*)src);
		}
		else
		if (prop.type == Clor)
		{
			memcpy(prop.value, src, sizeof(float) * 4);
		}
		else
		if (prop.type == Array)
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
		if (prop.type == Sprite)
		{
			Sprite::Copy((Sprite::Data*)src, (Sprite::Data*)prop.value);
		}
	}
}

void MetaData::BindToScript(class asIScriptEngine* engine, const char* script_class_name)
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
			engine->RegisterObjectProperty(script_class_name, decl, (int)prop.offset);
		}
		
	}
}

#ifdef EDITOR
EUIEditBox* MetaData::GetFloatEditBox(const char* name)
{
	for (auto& prop : properties)
	{
		if (StringUtils::IsEqual(prop.propName.c_str(), name))
		{
			if (prop.type == Float && prop.widgets.size() > 0)
			{
				return ((FloatWidget*)(prop.widgets.begin()->second))->ebox;
			}

			break;
		}
	}

	return nullptr;
}

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
			if (prop.type == Boolean)
			{
				widget = new BoolWidget();
			}
			else
			if (prop.type == Integer)
			{
				widget = new IntWidget();
			}
			else
			if (prop.type == Float)
			{
				widget = new FloatWidget();
			}
			else
			if (prop.type == String)
			{
				widget = new StringWidget();
			}
			else
			if (prop.type == FileName)
			{
				widget = new FileNameWidget();
			}
			else
			if (prop.type == Clor)
			{
				widget = new ColorWidget();
			}
			else
			if (prop.type == Enum)
			{
				widget = new EnumWidget(&enums[prop.defvalue.enumIndex]);
			}
			else
			if (prop.type == EnumString)
			{
				widget = new EnumStringWidget(prop.enum_callback);
			}
			else
			if (prop.type == Callback)
			{
				widget = new CallbackWidget();
			}
			else
			if (prop.type == Array)
			{
				widget = new ArrayWidget();
				((ArrayWidget*)widget)->prop = prop.adapter;
				((ArrayWidget*)widget)->root = root;
			}
			else
			if (prop.type == Sprite)
			{
				widget = new SpriteWidget();
			}

			widget->Init(parent, prop.catName.c_str(), prop.propName.c_str());
			prop.widgets[parent] = widget;
		}

		if (prop.type == Callback)
		{
			((CallbackWidget*)widget)->Prepare(owner, prop.callback);
		}
		else
		{
			widget->SetData(prop.value);
		}

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

			if (prop.type == Callback)
			{
				((CallbackWidget*)widget.second)->Prepare(owner, prop.callback);
			}
			else
			{
				widget.second->SetData(prop.value);
			}
		}
	}
}

bool MetaData::IsValueWasChanged()
{
	bool res = false;
	for (auto& prop : properties)
	{
		if (prop.type == Array)
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