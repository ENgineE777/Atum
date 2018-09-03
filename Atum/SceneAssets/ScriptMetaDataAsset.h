
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/SpriteAsset.h"
#include "Services/Script/Libs/scriptarray.h"

class ScriptMetaDataAsset : public SceneAsset
{
public:
	META_DATA_DECL(ScriptMetaDataAsset)

	enum Type
	{
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
