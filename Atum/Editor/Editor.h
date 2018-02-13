
#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Services/Scene/SceneObject.h"
#include "Services/Scene/SceneAsset.h"
#include <EUI.h>
#include "freecamera.h"
#include "Gizmo.h"
#include "Support/Timer.h"

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
	EUIListBox*  assetList;

	EUIEditBox*  objectName;

	EUIButton* moveBtn;
	EUIButton* rotateBtn;
	EUIButton* globalBtn;
	EUIButton* localBtn;
	EUIButton* playBtn;
	EUICategories* objCat;
	EUIPanel* viewport;
	EUIPanel* asset_viewport;

	EUILayout* viewport_lt;
	EUILayout* asset_vp_sheet_lt;

	EUIWindow* mainWnd;
	EUIWindow* gameWnd;

	Scene scene;
	Gizmo gizmo;

	Vector2 prev_mx;
	bool allowCopy = false;

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

	virtual void OnMouseMove(EUIWidget* sender, int mx, int my);
	virtual void OnLeftMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnMenuItem(EUIWidget* sender, int id);
	virtual void OnUpdate(EUIWidget* sender);
	virtual void OnListBoxChange(EUIWidget* sender, int index);
	virtual void OnEditBoxStopEditing(EUIWidget* sender);
	virtual void OnResize(EUIWidget* sender);
	virtual void OnWinClose(EUIWidget* sender);
};
