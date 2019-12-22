
#include "ScriptMetaDataComp.h"

COMPREG(ScriptMetaDataComp, "ScriptMetaData")
COMP_ASSETS()
COMPEXCL(ScriptMetaDataAsset)
COMPREG_END(ScriptMetaDataComp)

META_DATA_DESC(ScriptMetaDataComp)
SCENEOBJECT_PROP(ScriptMetaDataComp, asset_ref, "MetaData", "Asset")
META_DATA_DESC_END()

COMPREG(ScriptMetaDataCompInst, "ScriptMetaDataInst")
COMP_INSTANCES()
COMPREG_END(ScriptMetaDataCompInst)

META_DATA_DESC(ScriptMetaDataCompInst)
META_DATA_DESC_END()

ScriptMetaDataComp::ScriptMetaDataComp()
{
	inst_class_name = "ScriptMetaDataCompInst";
}

void ScriptMetaDataComp::Load(JSONReader& loader)
{
	SceneAssetComp::Load(loader);

	int count = 0;
	loader.Read("count", count);
	values.resize(count);

	for (auto& val : values)
	{
		if (!loader.EnterBlock("value")) break;

		loader.Read("type", (int&)val.type);

		switch (val.type)
		{
			case ScriptMetaDataAsset::Bool:  loader.Read("value", val.value.boolean); break;
			case ScriptMetaDataAsset::Int:   loader.Read("value", val.value.integer); break;
			case ScriptMetaDataAsset::Float: loader.Read("value", val.value.flt); break;
		}

		loader.LeaveBlock();
	}
}

void ScriptMetaDataComp::Save(JSONWriter& saver)
{
	SceneAssetComp::Save(saver);

	saver.Write("count", (int)values.size());

	saver.StartArray("value");

	for (auto& val : values)
	{
		saver.StartBlock(nullptr);
		saver.Write("type", val.type);

		switch (val.type)
		{
			case ScriptMetaDataAsset::Bool:  saver.Write("value", val.value.boolean); break;
			case ScriptMetaDataAsset::Int:   saver.Write("value", val.value.integer); break;
			case ScriptMetaDataAsset::Float: saver.Write("value", val.value.flt); break;
		}

		saver.FinishBlock();
	}

	saver.FinishArray();
}

void ScriptMetaDataComp::ApplyProperties()
{
	ScriptMetaDataAsset* prev_asset = asset;

	asset = (ScriptMetaDataAsset*)asset_ref.object;

	if (!asset)
	{
		values.clear();
	}
	else
	if ((asset && asset->properties.size() != values.size()) || (prev_asset && prev_asset != asset))
	{
		values.resize(asset->properties.size());

		int index = 0;
		for (auto& prop : asset->properties)
		{
			values[index].type = prop.type;
			
			switch (prop.type)
			{
				case ScriptMetaDataAsset::Bool  : values[index].value.boolean = false; break;
				case ScriptMetaDataAsset::Int   : values[index].value.integer = 0; break;
				case ScriptMetaDataAsset::Float : values[index].value.flt = 0.0f; break;
			}

			index++;
		}

#ifdef EDITOR
		if (saved_objCat && object->IsEditMode())
		{
			ShowPropWidgets(saved_objCat);
		}
#endif
	}
}

#ifdef EDITOR
void ScriptMetaDataComp::ShowPropWidgets(EUICategories* objCat)
{
	saved_objCat = objCat;

	auto meta = GetMetaData();

	for (auto prop : created_properties)
	{
		for (auto widget : prop.widgets)
		{
			widget.second->Release();
		}
	}

	created_properties.clear();

	if (objCat)
	{
		int index = 0;

		for (auto& val : values)
		{
			MetaData::Property prop;

			prop.offset = (uint8_t*)&values[index].value - (uint8_t*)this;
			prop.catName = "MetaData";
			prop.propName = asset->properties[index].name;

			switch (val.type)
			{
				case ScriptMetaDataAsset::Bool:
					prop.type = MetaData::Boolean;
					prop.defvalue.boolean = false;
					break;
				case ScriptMetaDataAsset::Int:
					prop.type = MetaData::Integer;
					prop.defvalue.integer = 0;
					break;
				case ScriptMetaDataAsset::Float:
					prop.type = MetaData::Float;
					prop.defvalue.flt = 0.0f;
					break;
			}

			meta->properties.push_back(prop);

			index++;
		}

		GetMetaData()->Prepare(this);
		GetMetaData()->PrepareWidgets(objCat);

		if (meta->properties.size() > 1)
		{
			for (int index = 1; index < meta->properties.size(); index++)
			{
				created_properties.push_back(meta->properties[index]);
			}

			meta->properties.erase(meta->properties.begin() + 1, meta->properties.end());
		}
	}
	else
	{
		GetMetaData()->HideWidgets();
	}
}

void ScriptMetaDataComp::Copy(SceneObjectComp* src)
{
	ScriptMetaDataComp* comp = (ScriptMetaDataComp*)src;

	values = comp->values;

	SceneAssetComp::Copy(src);
}
#endif

void ScriptMetaDataCompInst::InjectIntoScript(asIScriptObject* script_object, int index, const char* prefix)
{
	ScriptMetaDataComp* meta_data = (ScriptMetaDataComp*)asset_comp;

	if (meta_data->values.size() == 0)
	{
		return;
	}

	if (mapping.size() == 0)
	{
		mapping.resize(meta_data->values.size());

		for (int value_index = 0; value_index < meta_data->values.size(); value_index++)
		{
			mapping[value_index] = -1;

			for (int prop = 0; prop < (int)script_object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual(meta_data->asset->properties[value_index].name.c_str(), script_object->GetPropertyName(prop)))
				{
					mapping[value_index] = prop;
					break;
				}
			}
		}
	}

	for (int value_index = 0; value_index < meta_data->values.size(); value_index++)
	{
		int prop = mapping[value_index];

		if (prop == -1)
		{
			continue;
		}

		switch (meta_data->values[value_index].type)
		{
			case ScriptMetaDataAsset::Bool:
				if (script_object->GetPropertyTypeId(prop) == asTYPEID_BOOL)
				{
					*((bool*)script_object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.boolean;
				}
#ifdef EDITOR
				else
				{
					MessageBox(nullptr, StringUtils::PrintTemp("Property %s not set as bool for %s", meta_data->asset->properties[value_index].name.c_str(), object->GetName()), "Error on script inhection", MB_ICONERROR);
				}
#endif
				break;
			case ScriptMetaDataAsset::Int:
				if (script_object->GetPropertyTypeId(prop) == asTYPEID_INT32)
				{
					*((int*)script_object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.integer;
				}
#ifdef EDITOR
				else
				{
					MessageBox(nullptr, StringUtils::PrintTemp("Property %s not set as int for %s", meta_data->asset->properties[value_index].name.c_str(), object->GetName()), "Error on script inhection", MB_ICONERROR);
				}
#endif
				break;
			case ScriptMetaDataAsset::Float:
				if (script_object->GetPropertyTypeId(prop) == asTYPEID_FLOAT)
				{
					*((float*)script_object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.flt;
				}
#ifdef EDITOR
				else
				{
					MessageBox(nullptr, StringUtils::PrintTemp("Property %s not set as float for %s", meta_data->asset->properties[value_index].name.c_str(), object->GetName()), "Error on script inhection", MB_ICONERROR);
				}
#endif
				break;
		}
	}
}