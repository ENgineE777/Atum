
#include "SceneAsset.h"

#ifdef EDITOR
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

bool SceneAsset::UsingCamera2DPos()
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
	inst->cam2d_pos = cam2d_pos;
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

bool SceneAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index)
{
	return false;
}

void SceneAsset::OnAssetTreeSelChange(SceneAsset* item)
{
	auto kl = &SceneAsset::OnAssetTreePopupItem;
	(this->*kl)(0);
}

void SceneAsset::OnAssetTreePopupItem(int id)
{

}

void SceneAsset::OnAssetTreeRightClick(int x, int y, SceneAsset* item, int child_index)
{

}
#endif