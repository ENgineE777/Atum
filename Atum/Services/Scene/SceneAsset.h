
#pragma once

#include "SceneObject.h"

#ifdef EDITOR
#include "EUI.h"
#endif

/**
\ingroup gr_code_services_scene
*/

class SceneAsset : public SceneObject
{
	friend class Scene;

public:


	virtual ~SceneAsset() = default;

	bool UsingCamera2DPos() override;

	const char* inst_class_name = nullptr;

#ifdef EDITOR
	struct AssetInstance
	{
		string scene_path;
		uint32_t inst_uid = 0;
		uint32_t inst_child_uid = 0;
		bool is_asset = false;
		SceneObject* object = nullptr;

		AssetInstance() = default;
		AssetInstance(SceneObject* set_object);
		SceneObject* GetObject();
	};
	vector<AssetInstance> instances;

	void SetOwner(Scene* owner) override;
	bool IsAsset() override;
	virtual SceneObject* CreateInstance(Scene* scene);
	virtual void DeleteAsset(SceneObject* obj);

	virtual void PreapreAssetTree();
	virtual bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneAsset* item, int prev_child_index, SceneObject* target, int child_index);
	virtual void OnAssetTreeSelChange(SceneAsset* item);
	virtual void OnAssetTreePopupItem(int id);
	virtual void OnAssetTreeRightClick(int x, int y, SceneAsset* item, int child_index);
#endif
};

CLASSFACTORYDEF(SceneAsset)
CLASSFACTORYDEF_END()
