
#include "ScriptMetaDataComp.h"

COMPREG(ScriptMetaDataComp, "ScriptMetaData")
COMP_ASSETS()
COMPEXCL(ScriptMetaDataAsset)
COMPREG_END(ScriptMetaDataComp)

META_DATA_DESC(ScriptMetaDataComp)
STRING_PROP(ScriptMetaDataComp, asset_name, "", "MetaData", "AssetName")
META_DATA_DESC_END()

COMPREG(ScriptMetaDataCompInst, "ScriptMetaDataInst")
COMP_INSTANCES()
COMPREG_END(ScriptMetaDataCompInst)

META_DATA_DESC(ScriptMetaDataCompInst)
META_DATA_DESC_END()

ScriptMetaDataComp::ScriptMetaDataComp()
{
	inst_class_name = "ScriptMetaDataInst";
}

void ScriptMetaDataComp::Load(JSONReader& loader)
{
	loader.Read("AssetName", asset_name);

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
	saver.Write("AssetName", asset_name.c_str());

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

	asset = (ScriptMetaDataAsset*)object->GetScene()->FindByName(asset_name.c_str(), true);

	if (asset && asset->properties.size() != values.size() || (prev_asset && prev_asset != asset))
	{
		if (!asset)
		{
			values.clear();
		}
		else
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
		}
	}
}

void ScriptMetaDataComp::ShowPropWidgets(EUICategories* objCat)
{
	if (objCat)
	{
		auto meta = GetMetaData();

		meta->properties.clear();

		{
			MetaData::Property prop;
			prop.offset = memberOFFSET(ScriptMetaDataComp, asset_name);
			prop.type = MetaData::String;
			prop.defvalue.string = 0;
			prop.catName = "MetaData";
			prop.propName = "AssetName";
			meta->properties.push_back(prop);
		}

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