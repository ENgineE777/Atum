
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

		MenuCopyID = 1100,
		MenuDeleteID = 1101,

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
	bool       gizmoMove;
	bool       gizmoGlobal;

	std::string  sceneName;

	SceneObject* selectedObject;
	EUIListBox*  sceneList;

	SceneAsset*  selectedAsset = nullptr;
	SceneAsset*  selectedAsset2Drag = nullptr;
	EUITreeView* assets_treeview;

	EUIEditBox*  objectName;

	EUIButton* moveBtn;
	EUIButton* rotateBtn;
	EUIButton* globalBtn;
	EUIButton* localBtn;
	EUIButton* mode2DBtn;
	EUIButton* playBtn;
	EUICategories* objCat;
	EUIPanel* viewport;
	EUIPanel* asset_treeview_panel;
	EUITreeView* asset_treeview;
	EUIPanel* asset_viewport;

	EUILayout* viewport_lt;
	EUILayout* asset_vp_sheet_lt;

	EUIWindow* mainWnd;
	EUIWindow* gameWnd;
	EUIPanel* game_viewport = nullptr;

	map<int, string> id2name;

	Scene ed_scene;
	Scene* scene = nullptr;
	Gizmo gizmo;

	Vector2 prev_ms;
	bool allowCopy = false;

	bool asset_drag_as_inst = false;
	TaskExecutor::SingleTaskPool* renderTaskPool;

public:

	Editor();
	virtual ~Editor();

	void Init();
	int  Run();

	void ClearScene();
	void UpdateGizmoToolbar();
	void SelectObject(SceneObject* obj);
	void CopyObject(SceneObject* obj);
	void CreateSceneObject(const char* name);
	void DeleteSceneObject(SceneObject* obj);
	void SetUniqueName(SceneObject* obj, const char* name);
	void SelectAsset(SceneAsset* obj);
	void CopyAsset(SceneAsset* obj);
	void CreateSceneAsset(const char* name);
	void DeleteSceneAsset(SceneAsset* obj);
	void SetUniqueAssetName(SceneAsset* obj, const char* name);
	void ShowVieport();
	void StartScene();
	void StopScene();
	void Draw(float dt);

	void OnMouseMove(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnMenuItem(EUIMenu* sender, int id) override;
	void OnUpdate(EUIWidget* sender) override;
	void OnListBoxSelChange(EUIListBox* sender, int index) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
	void OnResize(EUIWidget* sender) override;
	void OnWinClose(EUIWidget* sender) override;
	void OnTreeViewItemDragged(EUITreeView* sender, EUITreeView* target, void* item, int prev_child_index, void* parent, int child_index) override;
	void OnTreeViewSelChange(EUITreeView* sender, void* item) override;
	void OnTreeReCreateItem(EUITreeView* sender, void* item, void* ptr) override;
	void OnTreeViewPopupItem(EUITreeView* sender, int id) override;
	void OnTreeViewRightClick(EUITreeView* sender, void* item, int child_index) override;
};
