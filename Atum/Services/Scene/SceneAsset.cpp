
#include "SceneAsset.h"

SceneAsset::SceneAsset() : SceneObject()
{
	taskPool = taskExecutor.CreateSingleTaskPool();
	renderTaskPool = render.AddTaskPool();

	taskPool->SetActive(false);
	renderTaskPool->SetActive(false);
}

SceneAsset::~SceneAsset()
{

}

TaskExecutor::SingleTaskPool* SceneAsset::Tasks()
{
	return taskPool;
}

TaskExecutor::SingleTaskPool* SceneAsset::RenderTasks()
{
	return renderTaskPool;
}

void SceneAsset::EnableTasks(bool enable)
{
	taskPool->SetActive(enable);
	renderTaskPool->SetActive(enable);
}

#ifdef EDITOR
bool SceneAsset::PrepareWidgets(EUITreeView* asset_treeview, EUICategories* cat, EUIEditBox* obj_name)
{
	return false;
}

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

void SceneAsset::OnAssetTreeRightClick(SceneAsset* item, int child_index)
{

}
#endif