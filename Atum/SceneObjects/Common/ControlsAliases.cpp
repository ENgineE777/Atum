#include "ControlsAliases.h"

CLASSREG(SceneObject, ControlsAliases, "ControlsAliases")

META_DATA_DESC(ControlsAliases)
BASE_SCENE_OBJ_PROP(ControlsAliases)
FILENAME_PROP(ControlsAliases, aliases_name, "", "Property", "Aliases")
META_DATA_DESC_END()

void ControlsAliases::Init()
{
}

bool ControlsAliases::Play()
{
	core.controls.LoadAliases(aliases_name.c_str());

	return true;
}