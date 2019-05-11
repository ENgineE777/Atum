
#pragma once

#include "Services/Scene/SceneObjectComp.h"
#include "ScriptMetaDataAsset.h"

/**
\page scene_assets_components_ScriptMetaData Script meta data component

This component allows inject additional fields into script for a scene object. Also
it is possible to set value for each parametrs in Script Meta Data Asset.

This class ::ScriptMetaDataAsset is a representation on C++ side.

Parameters
----------

Name              | Description
------------------| -------------
Asset             | Asset that holds description of additional properties


*/


/**
\ingroup gr_code_scene_assets_components
*/

/**
\brief Representation of script meta data component

This component allows inject additional fields into script for a scene object. Also
it is possible to set value for each parametrs in Script Meta Data Asset.

*/

class ScriptMetaDataComp : public SceneAssetComp
{
public:

#ifndef DOXYGEN_SKIP
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

#endif
};

class ScriptMetaDataCompInst : public SceneObjectInstComp
{
public:
	vector<int> mapping;
	META_DATA_DECL(ScriptMetaDataComp)
	void InjectIntoScript(asIScriptObject* object, int index, const char* prefix) override;
};