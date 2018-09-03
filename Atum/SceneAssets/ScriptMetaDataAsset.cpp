
#include "ScriptMetaDataAsset.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneAsset, ScriptMetaDataAsset, "ScriptMetaData")

META_DATA_DESC(ScriptMetaDataAsset::MetaType)
ENUM_PROP(ScriptMetaDataAsset::MetaType, type, 0, "Prop", "type")
	ENUM_ELEM("Int", 0)
	ENUM_ELEM("Float", 1)
ENUM_END
STRING_PROP(ScriptMetaDataAsset::MetaType, name, "", "Prop", "name")
META_DATA_DESC_END()

META_DATA_DESC(ScriptMetaDataAsset)
BASE_SCENE_OBJ_NAME_PROP(ScriptMetaDataAsset)
BASE_SCENE_OBJ_STATE_PROP(ScriptMetaDataAsset)
ARRAY_PROP(ScriptMetaDataAsset, properties, MetaType, "Prop", "properties")
META_DATA_DESC_END()

void ScriptMetaDataAsset::Init()
{

}