
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Services/Script/Libs/scriptarray.h"

/**
\page scene_assets_common_ScriptMetaDataAsset Script Meta Data

Script Meta Data

*/


/**
\ingroup gr_code_scene_assets_common
*/

class ScriptMetaDataAsset : public SceneAsset
{
public:
	META_DATA_DECL(ScriptMetaDataAsset)

	enum Type
	{
		Bool,
		Int,
		Float
	};

	struct MetaType
	{
		META_DATA_DECL(MetaType)

		Type type;
		string name;
	};

	vector<MetaType> properties;

	virtual ~ScriptMetaDataAsset() = default;

	void Init() override;
};


#define CLASSREG(baseClass, className, shortName)\
CLASSREGEX(baseClass, className, className, shortName)\
CLASSREGEX_END(baseClass, className)
