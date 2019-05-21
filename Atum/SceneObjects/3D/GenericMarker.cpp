
#include "GenericMarker.h"

CLASSREG(SceneObject, GenericMarker, "GenericMarker")

META_DATA_DESC(GenericMarker)
BASE_SCENE_OBJ_PROP(GenericMarker)
STRING_PROP(GenericMarker, scene_group, "", "Prop", "scene_group")
META_DATA_DESC_END()

Matrix& GenericMarker::Trans()
{
	return transform;
}

bool GenericMarker::Is3DObject()
{
	return true;
}

void GenericMarker::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&GenericMarker::Draw);
}

void GenericMarker::ApplyProperties()
{
	GetScene()->DelFromAllGroups(this);

	if (scene_group.c_str()[0] != 0)
	{
		GetScene()->AddToGroup(this, scene_group.c_str());
	}
}

void GenericMarker::Draw(float dt)
{
	if (GetScene()->Playing())
	{
		return;
	}

	core.render.DebugSphere(transform.Pos(), COLOR_WHITE, 1.0f);
}