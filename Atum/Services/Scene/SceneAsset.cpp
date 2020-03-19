
#include "SceneAsset.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#include "Editor/Project.h"

SceneAsset::AssetInstance::AssetInstance(SceneObject* set_object)
{
	scene_path = set_object->GetScene()->GetPath();
	scene_path += "/";
	scene_path += set_object->GetScene()->GetName();
	scene_path += ".sca";

	inst_uid = set_object->GetUID();
	object = set_object;
}

SceneObject* SceneAsset::AssetInstance::GetObject()
{
	if (!object)
	{
		object = project.GetScene(scene_path.c_str())->FindByUID(inst_uid, inst_child_uid, is_asset);
	}

	return object;
}
#endif

bool SceneAsset::UsingOwnCamera()
{
	return true;
}

#ifdef EDITOR
void SceneAsset::SetScene(Scene* set_scene)
{
	SceneObject::SetScene(set_scene);

	for (auto& inst : instances)
	{
		((SceneObjectInst*)inst.GetObject())->asset_uid = GetUID();
	}
}

bool SceneAsset::IsAsset()
{
	return true;
}

SceneObject* SceneAsset::CreateInstance(Scene* scene)
{
	SceneObjectInst* inst = (SceneObjectInst*)scene->CreateObject(inst_class_name, false);
	scene->GenerateUID(inst, false);

	inst->asset_uid = GetUID();
	inst->asset = this;

	if (inst->UsingOwnCamera())
	{
		if (inst->Is3DObject())
		{
			inst->camera3d_pos = editor.freecamera.pos;
			inst->camera3d_angles = editor.freecamera.angles;
		}
		else
		{
			inst->camera2d_pos = camera2d_pos;
			inst->camera2d_zoom = camera2d_zoom;
		}
	}

	inst->ApplyProperties();
	inst->SetName(GetName());

	instances.push_back(AssetInstance(inst));

	return inst;
}

void SceneAsset::DeleteAsset(SceneObject* obj)
{
	int index = 0;

	for (auto& inst : instances)
	{
		if (inst.GetObject() == obj)
		{
			instances.erase(instances.begin() + index);
			break;
		}

		index++;
	}
}

void SceneAsset::PreapreAssetTree()
{

}

bool SceneAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneObject* item, int prev_child_index, SceneObject* target, int child_index)
{
	return false;
}

void SceneAsset::OnAssetTreeSelChange(SceneObject* item)
{
}

void SceneAsset::OnAssetTreePopupItem(int id)
{

}

void SceneAsset::OnAssetTreeRightClick(int x, int y, SceneObject* item, int child_index)
{

}
#endif