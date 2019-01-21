
#pragma once

#include "Support/Support.h"
#include "Services/Core/Core.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Scene/SceneAsset.h"
#include <EUI.h>
#include "freecamera.h"
#include "Gizmo.h"
#include "EditorDrawer.h"
#include <map>
#include "SceneObjCmpWidget.h"
#include "Project.h"

class Editor : public Object, public EUIWidget::Listener
{
	friend class Project;
	friend class LayerEntryWidget;

	enum Const
	{
		MenuNewID = 1000,
		MenuOpenID = 1001,
		MenuSaveID = 1002,
		MenuSaveAsID = 1003,
		MenuExitID = 1004,
		MenuSceneObjectID = 10000,
		MenuSceneAssetID = 15000,
		MenuLastSceneID = 19999
	};

	FreeCamera freecamera;
	bool       gizmoMove = true;
	bool       gizmoGlobal = true;

	EUITreeView* scene_treeview = nullptr;

	SceneObject* selectedObject = nullptr;
	bool         isSelectedAsset = false;
	EUITreeView* project_treeview = nullptr;

	EUITabSheet* scene_sheet = nullptr;
	EUITreeView* assets_treeview = nullptr;

	static EUITabPanel* outputPanels;
	static map<string, EUIListBox*> output_boxes;

	EUIWidget* popup_parent = nullptr;
	EUIMenu* popup_menu = nullptr;
	EUIButton* moveModeBtn = nullptr;

	EUIPanel* trans3d_gizmo = nullptr;

	EUIButton* moveBtn = nullptr;
	EUIButton* rotateBtn = nullptr;
	EUIButton* globalBtn = nullptr;
	EUIButton* localBtn = nullptr;

	EUIPanel* trans2d_gizmo = nullptr;

	EUIEditBox* x_align = nullptr;
	EUIEditBox* y_align = nullptr;

	EUIButton* playBtn = nullptr;
	EUICategories* objCat = nullptr;
	SceneObjCmpWidget objCmpWgt;
	EUIPanel* viewport = nullptr;
	EUIPanel* asset_treeview_panel = nullptr;
	EUITreeView* asset_treeview = nullptr;

	EUIEditBox*    layers_ebox = nullptr;
	EUIButton*     layers_add_btn = nullptr;
	EUICategories* layers_cat = nullptr;

	EUIListBox* groups_list = nullptr;
	EUIEditBox* groups_ebox = nullptr;
	EUIButton*  groups_add_btn = nullptr;
	EUIButton*  groups_del_btn = nullptr;

	EUILayout* vp_sheet_lt = nullptr;

	EUIWindow* mainWnd = nullptr;
	EUIWindow* gameWnd = nullptr;
	EUIPanel* game_viewport = nullptr;

	Scene* scene = nullptr;
	Gizmo gizmo;

	Vector2 prev_ms;
	Vector2 ms_rect;
	bool allowCopy = false;
	bool rect_select = false;

	int asset_panel_width = 1;
	void*        popup_item = nullptr;
	SceneObject* popup_scene_item = nullptr;
	int          popup_child_index = -1;
	SceneObject* object_to_copy = nullptr;
	bool asset_drag_as_inst = false;
	bool allow_delete_objects_by_tree = false;
	TaskExecutor::SingleTaskPool* renderTaskPool;

	bool in_select_object = false;

public:

	~Editor() = default;

	void Init();
	int  Run();

	static void LogToOutputBox(const char* name, const char* text);
	static void AddOutputBox(const char* name);
	void ShowGizmoControls(int mode);
	void ClearScene();
	void UpdateGizmoToolbar();
	void SelectObject(SceneObject* obj, bool is_asset);
	void CopyObject(SceneObject* obj, void* parent, bool is_asset);
	void CreateSceneObject(const char* name, void* parent, bool is_asset);
	void ShowVieport();
	void StartScene();
	void StopScene();
	void Draw(float dt);
	void CreatePopup(EUITreeView* treeview, int x, int y, bool is_asset);
	void ProcessTreeviewPopup(EUITreeView* treeview, int id, bool is_asset);

	void OnMouseMove(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnRightMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnEditBoxStopEditing(class EUIEditBox* sender) override;
	void OnMenuItem(EUIMenu* sender, int id) override;
	void OnUpdate(EUIWidget* sender) override;
	void OnResize(EUIWidget* sender) override;
	void OnWinClose(EUIWidget* sender) override;
	bool OnTreeViewItemDragged(EUITreeView* sender, EUIWidget* target, void* item, int prev_child_index, void* parent, int child_index) override;
	void OnTreeViewSelChange(EUITreeView* sender, void* item) override;
	void OnTreeReCreateItem(EUITreeView* sender, void* item, void* ptr) override;
	void OnTreeDeleteItem(class EUITreeView* sender, void* item, void* ptr) override;
	void OnTreeViewRightClick(EUITreeView* sender, int x, int y, void* item, int child_index) override;
	void OnTreeViewSelItemTextChanged(class EUITreeView* sender, void* item, const char* text) override;
};

extern Editor editor;