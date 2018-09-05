
#pragma once

#include "Services/Scene/SceneObjectComp.h"
#include "ScriptMetaDataAsset.h"

class ScriptMetaDataComp : public SceneAssetComp
{
public:

	ScriptMetaDataComp();

	META_DATA_DECL(ScriptMetaDataComp)

	union Value
	{
		bool  boolean;
		int   integer;
		float flt;
	};

	struct TypedValue
	{
		ScriptMetaDataAsset::Type type;
		Value value;
	};

	vector<TypedValue> values;
	string asset_name;
	ScriptMetaDataAsset* asset = nullptr;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;
	void ApplyProperties() override;
	void ShowPropWidgets(EUICategories* objCat) override;
};

class ScriptMetaDataCompInst : public SceneObjectInstComp
{
public:

	META_DATA_DECL(ScriptMetaDataComp)
};