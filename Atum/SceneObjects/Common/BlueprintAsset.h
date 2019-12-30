
#pragma once

#include "Services/Scene/SceneAsset.h"

class BlueprintAsset : public SceneAsset
{
public:
	META_DATA_DECL(BlueprintAsset)

	std::vector<SceneObject*> childs;
	Scene sub_scene;

	BlueprintAsset();
	~BlueprintAsset() = default;
	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;

	SceneObject* GetChild(uint32_t uid) override;

#ifdef EDITOR
	SceneObject* popup_item = nullptr;
	static SceneObject* sel_inst;
	SceneObject* CreateInstance(Scene* scene) override;
	void Copy(SceneObject* src) override;

	bool UseAseetsTree() override;
	void SetEditMode(bool ed) override;
	void PreapreAssetTree() override;
	bool OnAssetTreeViewItemDragged(bool item_from_assets, SceneObject* item, int prev_child_index, SceneObject* target, int child_index) override;
	void OnAssetTreeSelChange(SceneObject* item) override;
	void OnAssetTreePopupItem(int id) override;
	void OnAssetTreeRightClick(int x, int y, SceneObject* item, int child_index) override;
	void CheckProperties() override;
	void OnDragObjectFromTreeView(bool is_assets_tree, SceneObject* object, Vector2 ms) override;
	void OnMouseMove(Vector2 delta_ms) override;
	void OnLeftMouseDown(Vector2 ms) override;
	void OnLeftMouseUp() override;

	SceneObject* CreateObject(const char* type);
	void SaveInstData(JSONWriter& writer, SceneObject* inst);
	void SaveAssetData(JSONWriter& writer) override;
#endif

	void Work(float dt);
	void Release() override;
};
