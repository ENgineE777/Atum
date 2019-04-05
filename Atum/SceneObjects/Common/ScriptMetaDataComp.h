
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
	SceneObjectRef asset_ref;

	ScriptMetaDataAsset* asset = nullptr;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;
	void ApplyProperties() override;

#ifdef EDITOR
	EUICategories* saved_objCat = nullptr;
	void ShowPropWidgets(EUICategories* objCat) override;
#endif
};

class ScriptMetaDataCompInst : public SceneObjectInstComp
{
public:
	vector<int> mapping;
	META_DATA_DECL(ScriptMetaDataComp)
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;
};