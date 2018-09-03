
#include "ScriptMetaDataComp.h"
#include "ScriptMetaDataAsset.h"

COMPREG(ScriptMetaDataComp, "ScriptMetaData")
COMP_ASSETS()
COMPEXCL(ScriptMetaDataAsset)
COMPREG_END(ScriptMetaDataComp)

META_DATA_DESC(ScriptMetaDataComp)
META_DATA_DESC_END()

COMPREG(ScriptMetaDataCompInst, "ScriptMetaDataInst")
COMP_INSTANCES()
COMPREG_END(ScriptMetaDataCompInst)

META_DATA_DESC(ScriptMetaDataCompInst)
META_DATA_DESC_END()

void ScriptMetaDataComp::Init()
{
	inst_class_name = ClassFactorySceneObjectComp::Find("ScriptMetaDataInst")->GetName();
}

SceneObjectInstComp* ScriptMetaDataComp::CreateInstance()
{
	SceneObjectInstComp* inst = (SceneObjectInstComp*)ClassFactorySceneObjectComp::Create(inst_class_name);
	inst->class_name = inst_class_name;
	inst->Init();

	return inst;
}