
#include "SceneAsset.h"

SceneObject* SceneAsset::CreateInstance()
{
	return nullptr;
}

#ifdef EDITOR
bool SceneAsset::OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index)
{
	return false;
}

void SceneAsset::OnAssetTreeSelChange(SceneAsset* item)
{

}

void SceneAsset::OnAssetTreePopupItem(int id)
{

}

void SceneAsset::OnAssetTreeRightClick(int x, int y, SceneAsset* item, int child_index)
{

}
#endif