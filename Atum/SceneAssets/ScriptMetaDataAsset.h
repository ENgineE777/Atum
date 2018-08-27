
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

		int type;
		string name;
	};

	vector<MetaType> metas;

	virtual ~ScriptMetaDataAsset() = default;

	void Init() override;
	void Draw(float dt);
};
