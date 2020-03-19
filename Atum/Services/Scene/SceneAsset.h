
#pragma once

#include "SceneObject.h"

#ifdef EDITOR
#include "EUI.h"
#endif

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneAsset

This is scene asset which is a base class of asset in a scene.

*/

class SceneAsset : public SceneObject
{
	friend class Scene;

public:

#ifndef DOXYGEN_SKIP
	virtual ~SceneAsset() = default;

	bool UsingOwnCamera() override;

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

	void SetScene(Scene* scene) override;
	bool IsAsset() override;
	virtual SceneObject* CreateInstance(Scene* scene);
	virtual void DeleteAsset(SceneObject* obj);

	virtual void PreapreAssetTree();
	virtual bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneObject* item, int prev_child_index, SceneObject* target, int child_index);
	virtual void OnAssetTreeSelChange(SceneObject* item);
	virtual void OnAssetTreePopupItem(int id);
	virtual void OnAssetTreeRightClick(int x, int y, SceneObject* item, int child_index);
#endif

#endif
};

CLASSFACTORYDEF(SceneAsset)
CLASSFACTORYDEF_END()
