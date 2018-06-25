
#pragma once

#include "Support/Support.h"
#include "Services/Core/Core.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Scene/SceneAsset.h"
#include <EUI.h>
#include "freecamera.h"
#include "Gizmo.h"
#include <map>

class Editor : public Object, public EUIWidget::Listener
{
	enum Const
	{
		MenuNewID = 1000,
		MenuOpenID = 1001,
		MenuSaveID = 1002,
		MenuSaveAsID = 1003,
		MenuExitID = 1004,

		ViewportID = 1500,
		MoveBtnID = 1501,
		RotateBtnID = 1502,
		GlobalBtnID = 1503,
		LocalBtnID = 1504,
		PlayBtnID = 1505,
		GameViewportID = 1506,
		AssetViewportID = 1507,
		SceneListID = 1508,
		AssetListID = 1509,

		MenuSceneObjectID = 10000,
		MenuSceneAssetID = 15000
	};

	FreeCamera freecamera;
	bool       gizmoMove = true;
	bool       gizmoGlobal = true;

	std::string  sceneName;

	SceneObject* selectedObject = nullptr;
	bool         isSelectedAsset = false;
	EUITreeView* scene_treeview = nullptr;

	SceneAsset*  selectedAsset2Drag = nullptr;
	EUITreeView* assets_treeview = nullptr;

	EUIEditBox*  objectName = nullptr;

	static EUITabPanel* outputPanels;
	static map<string, EUIListBox*> output_boxes;

	EUIButton* moveBtn = nullptr;
	EUIButton* rotateBtn = nullptr;
	EUIButton* globalBtn = nullptr;
	EUIButton* localBtn = nullptr;
	EUIButton* mode2DBtn = nullptr;
	EUIButton* playBtn = nullptr;
	EUICategories* objCat = nullptr;
	EUIPanel* viewport = nullptr;
	EUIPanel* asset_treeview_panel = nullptr;
	EUITreeView* asset_treeview = nullptr;
	EUIPanel* asset_viewport = nullptr;

	EUILayout* viewport_lt = nullptr;
	EUILayout* asset_vp_sheet_lt = nullptr;

	EUIWindow* mainWnd = nullptr;
	EUIWindow* gameWnd = nullptr;
	EUIPanel* game_viewport = nullptr;

	Scene ed_scene;
	Scene* scene = nullptr;
	Gizmo gizmo;

	Vector2 prev_ms;
	bool allowCopy = false;

	void*        popup_item = nullptr;
	SceneObject* popup_scene_item = nullptr;
	int          popup_child_index = -1;
	SceneObject* object_to_copy = nullptr;
	bool asset_drag_as_inst = false;
	bool allow_delete_objects_by_tree = false;
	TaskExecutor::SingleTaskPool* renderTaskPool;

	struct SceneTreeNode
	{
		int type = 0;
		string name;
		int index_in_scene = -1;
	};

	bool in_select_object = false;
	vector<SceneTreeNode> scene_nodes;
	vector<SceneTreeNode> assets_nodes;

	Texture* checker_texture = nullptr;

public:

	~Editor() = default;

	void Init();
	int  Run();

	static void LogToOutputBox(const char* name, const char* text);
	static void AddOutputBox(const char* name);
	void ClearScene();
	void UpdateGizmoToolbar();
	void SelectObject(SceneObject* obj, bool is_asset);
	void CopyObject(SceneObject* obj, void* parent, bool is_asset);
	void CreateSceneObject(const char* name, void* parent, bool is_asset);
	void DeleteSceneObject(SceneObject* obj);
	void SetUniqueName(SceneObject* obj, const char* name, bool is_asset);
	void DeleteSceneAsset(SceneAsset* obj);
	void ShowVieport();
	void StartScene();
	void StopScene();
	void Draw(float dt);
	void Load();
	void Save();
	void LoadNodes(JSONReader* reader, vector<SceneTreeNode>& nodes, const char* group);
	void SaveNode(JSONWriter* writer, vector<SceneTreeNode>& nodes, const char* group);
	void RestoreTreeviewNodes();
	void RestoreTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& nodes, bool is_asset);
	void RestoreTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& nodes, int& index, void* item, bool is_asset);
	void GrabTreeviewNodes();
	void GrabTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& scene_nodes, void* item, bool is_asset);
	void CreatePopup(EUITreeView* treeview, bool is_asset);
	void ProcesTreeviewPopup(EUITreeView* treeview, int id, bool is_asset);

	void OnMouseMove(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnMenuItem(EUIMenu* sender, int id) override;
	void OnUpdate(EUIWidget* sender) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
	void OnResize(EUIWidget* sender) override;
	void OnWinClose(EUIWidget* sender) override;
	bool OnTreeViewItemDragged(EUITreeView* sender, EUITreeView* target, void* item, int prev_child_index, void* parent, int child_index) override;
	void OnTreeViewSelChange(EUITreeView* sender, void* item) override;
	void OnTreeReCreateItem(EUITreeView* sender, void* item, void* ptr) override;
	void OnTreeDeleteItem(class EUITreeView* sender, void* item, void* ptr) override;
	void OnTreeViewPopupItem(EUITreeView* sender, int id) override;
	void OnTreeViewRightClick(EUITreeView* sender, void* item, int child_index) override;
	void OnTreeViewSelItemTextChanged(class EUITreeView* sender, void* item, const char* text) override;
};
