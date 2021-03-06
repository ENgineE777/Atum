
#include "MeshAsset.h"

CLASSREG(SceneAsset, MeshAsset, "Mesh")

#ifdef EDITOR
#include "Editor/Editor.h"

void MeshResetView(void* owner)
{
	MeshAsset* mesh = (MeshAsset*)owner;

	mesh->ResetView();
}
#endif

META_DATA_DESC(MeshAsset)
	BASE_SCENE_OBJ_PROP(MeshAsset)
	FILENAME_PROP(MeshAsset, mesh_name, "", "Geometry", "Mesh")
#ifdef EDITOR
	CALLBACK_PROP(MeshAsset, MeshResetView, "Prop", "ResetView")
#endif
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

#ifdef EDITOR
	if (IsEditMode())
	{
		ResetView();
	}
#endif
}

void MeshAsset::Draw(float dt)
{
	if (instance)
	{
		instance->transform = trans;
	}
}

#ifdef EDITOR
void MeshAsset::ResetView()
{
	if (instance)
	{
		trans.Identity();
		instance->transform = trans;

		auto center = (instance->GetBBMax() + instance->GetBBMin()) * 0.5f;
		auto size = (instance->GetBBMax() - instance->GetBBMin()) * 0.5f;

		editor.freecamera.pos = Vector3(-15.0f, 7.0f, 0.0f) + center;
		editor.freecamera.angles = Vector2(0.0f, -0.56f);
	}
}
#endif