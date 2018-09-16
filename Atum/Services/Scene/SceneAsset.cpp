
#include "SceneAsset.h"

bool SceneAsset::UsingCamera2DPos()
{
	return true;
}

#ifdef EDITOR
bool SceneAsset::IsAsset()
{
	return true;
}

SceneObject* SceneAsset::CreateInstance()
{
	SceneObjectInst* inst = (SceneObjectInst*)owner->AddObject(inst_class_name, false);
	inst->asset_uid = GetUID();
	inst->asset = this;
	inst->ApplyProperties();
	inst->SetName(GetName());

	instances.push_back(inst);

	return inst;
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