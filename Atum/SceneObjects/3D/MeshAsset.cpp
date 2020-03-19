
#include "MeshAsset.h"

CLASSREG(SceneAsset, MeshAsset, "Mesh")

META_DATA_DESC(MeshAsset)
	BASE_SCENE_OBJ_PROP(MeshAsset)
	FILENAME_PROP(MeshAsset, mesh_name, "", "Geometry", "Mesh")
META_DATA_DESC_END()

MeshAsset::MeshAsset() : SceneAsset()
{
	inst_class_name = "MeshInstance";
}

Matrix* MeshAsset::Trans()
{
	return &trans;
}

bool MeshAsset::Is3DObject()
{
	return true;
}

void MeshAsset::Init()
{
	Tasks(true)->AddTask(100, this, (Object::Delegate)&MeshAsset::Draw);
}

void MeshAsset::ApplyProperties()
{
	RELEASE(instance)
	instance = core.meshes.LoadMesh(mesh_name.c_str(), RenderTasks(true));
}

void MeshAsset::Draw(float dt)
{
	if (instance)
	{
		instance->transform = trans;
	}
}