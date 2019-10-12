
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
		if (object->IsEditMode())
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

	if (meta->properties.size() > 1)
	{
		meta->properties.erase(meta->properties.begin() + 1, meta->properties.end());
	}

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
	}

	SceneAssetComp::ShowPropWidgets(objCat);
}
#endif

void ScriptMetaDataCompInst::InjectIntoScript(asIScriptObject* object, int index, const char* prefix)
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

			for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual(meta_data->asset->properties[value_index].name.c_str(), object->GetPropertyName(prop)))
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
				*((bool*)object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.boolean;
				break;
			case ScriptMetaDataAsset::Int:
				*((int*)object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.integer;
				break;
			case ScriptMetaDataAsset::Float:
				*((float*)object->GetAddressOfProperty(prop)) = meta_data->values[value_index].value.flt;
				break;
		}
	}
}