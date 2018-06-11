
#pragma once

#include "SceneObject.h"

#ifdef EDITOR
#include "EUI.h"
#endif

class SceneAsset : public SceneObject
{
	friend class Scene;

	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderTaskPool;

public:

	SceneAsset();
	virtual ~SceneAsset();

	virtual TaskExecutor::SingleTaskPool* Tasks();
	virtual TaskExecutor::SingleTaskPool* RenderTasks();
	virtual void EnableTasks(bool enable);

#ifdef EDITOR
	virtual bool PrepareWidgets(EUITreeView* asset_treeview, EUICategories* cat, EUIEditBox* obj_name);
	virtual bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index);
	virtual void OnAssetTreeSelChange(SceneAsset* item);
	virtual void OnAssetTreeReCreateItem(void* item, void* ptr);
	virtual void OnAssetTreePopupItem(int id);
	virtual void OnAssetTreeRightClick(SceneAsset* item, int child_index);
#endif
};

CLASSFACTORYDEF(SceneAsset)
