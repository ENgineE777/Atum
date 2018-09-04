
#include "ScriptMetaDataComp.h"
#include "ScriptMetaDataAsset.h"

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
	SceneAssetComp::Load(loader);
}

void ScriptMetaDataComp::Save(JSONWriter& saver)
{
	SceneAssetComp::Save(saver);
}

void ScriptMetaDataComp::ApplyProperties()
{
	asset = (ScriptMetaDataAsset*)object->GetScene()->FindByName(asset_name.c_str(), true);
}