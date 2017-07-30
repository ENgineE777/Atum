
#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Services/Scene/Scene.h"
#include <EUI.h>
#include "freecamera.h"
#include "Gizmo.h"
#include "Support/Timer.h"

class Editor : public Object
{
	enum Const
	{
		MenuNewID = 1000,
		MenuOpenID = 1001,
		MenuSaveID = 1002,
		MenuSaveAsID = 1003,
		MenuExitID = 1003,

		MenuDeleteID = 1100,

		MenuSceneObjectID = 1400,
		ViewportID = 1500,
		MoveBtnID = 1501,
		RotateBtnID = 1502,
		GlobalBtnID = 1503,
		LocalBtnID = 1504,
	};

	class Listener : public EUIWidget::Listener
	{
		Editor* owner;
	public:
		Listener(Editor* owner);

		virtual void OnMouseMove(EUIWidget* sender, int mx, int my);
		virtual void OnLeftMouseDown(EUIWidget* sender, int mx, int my);
		virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
		virtual void OnRightMouseUp(EUIWidget* sender, int mx, int my);
		virtual void OnMenuItem(EUIWidget* sender, int id);
		virtual void OnUpdate(EUIWidget* sender);
		virtual void OnListBoxChange(EUIWidget* sender, int index);
		virtual void OnEditBoxChange(EUIWidget* sender);
		virtual void OnResize(EUIWidget* sender);
	};

	FreeCamera freecamera;
	Listener   listener;
	float      prev_t;
	bool       gizmoMove;
	bool       gizmoGlobal;

	SceneObject* selectedObject;
	std::string  sceneName;
	EUIListBox*  sceneList;
	EUIEditBox*  objectName;
	EUIButton* moveBtn;
	EUIButton* rotateBtn;
	EUIButton* globalBtn;
	EUIButton* localBtn;
	EUICategories* objCat;

	Scene scene;
	Gizmo gizmo;

public:

	Editor();
	virtual ~Editor();

	void Init();
	int  Run();

	void ClearScene();
	void UpdateGizmoToolbar();
	void SelectObject(SceneObject* obj);
	void CreateSceneObject(const char* name);
	void DeleteSceneObject(SceneObject* obj);
	void SetUniqueName(SceneObject* obj, const char* name);
	void OnObjectNameChanged();
	void ProcessMenu(int id);
	void OnResize(int width, int height);
	void Update();
	void Render(float dt);
};
