
#pragma once

#include "Services/Scene/SceneObjectComp.h"

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

	vector<Value> values;
	string asset_name;
	class ScriptMetaDataAsset* asset = nullptr;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;
	void ApplyProperties() override;
};

class ScriptMetaDataCompInst : public SceneObjectInstComp
{
public:

	META_DATA_DECL(ScriptMetaDataComp)
};