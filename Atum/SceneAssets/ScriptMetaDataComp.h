
#pragma once

#include "Services/Scene/SceneObjectComp.h"

class ScriptMetaDataComp : public SceneAssetComp
{
public:

	META_DATA_DECL(ScriptMetaDataComp)

	void Init() override;
	SceneObjectInstComp* CreateInstance() override;
};

class ScriptMetaDataCompInst : public SceneObjectInstComp
{
public:

	META_DATA_DECL(ScriptMetaDataComp)
};