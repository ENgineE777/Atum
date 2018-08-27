
#include "ScriptMetaDataAsset.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneAsset, ScriptMetaDataAsset, "ScriptMetaData")

META_DATA_DESC(ScriptMetaDataAsset::MetaType)
INT_PROP(ScriptMetaDataAsset::MetaType, type, 10, "Prop", "type")
STRING_PROP(ScriptMetaDataAsset::MetaType, name, "", "Prop", "name")
META_DATA_DESC_END()

META_DATA_DESC(ScriptMetaDataAsset)
BASE_SCENE_OBJ_NAME_PROP(ScriptMetaDataAsset)
BASE_SCENE_OBJ_STATE_PROP(ScriptMetaDataAsset)
ARRAY_PROP(ScriptMetaDataAsset, metas, MetaType, "Prop", "metas")
META_DATA_DESC_END()

void ScriptMetaDataAsset::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&ScriptMetaDataAsset::Draw);
}

void ScriptMetaDataAsset::Draw(float dt)
{
	float x = 10.0f;
	for (auto& m : metas)
	{
		render.DebugSprite(nullptr, { (float)x, 30.0f }, { (float)m.type, (float)m.type }, COLOR_GREEN);
		x += m.type;
	}
}