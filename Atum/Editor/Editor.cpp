
#include "Editor.h"

Editor::Listener::Listener(Editor* set_owner)
{
	owner = set_owner;
}

void Editor::Listener::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		controls.OverrideMousePos(mx, my);
	}

	if (sender->GetID() == Editor::ViewportID)
	{
		if (owner->selectedObject && !owner->scene.Playing())
		{
			owner->gizmo.OnMouseMove((float)mx / (float)sender->GetWidth(),
										(float)my / (float)sender->GetHeight());

			if (allowCopy && controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active))
			{
				if (!owner->selectedObject->Trans().IsEqual(owner->gizmo.transform))
				{
					allowCopy = false;
					owner->CopyObject(owner->selectedObject);
				}
			}

			owner->selectedObject->Trans() = owner->gizmo.transform;
		}
	}
}

void Editor::Listener::OnLeftMouseDown(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID)
	{
		allowCopy = true;
		owner->gizmo.OnLeftMouseDown((float)mx / (float)sender->GetWidth(),
										(float)my / (float)sender->GetHeight());
	}
}

void Editor::Listener::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID)
	{
		owner->gizmo.OnLeftMouseUp();
	}

	if (sender->GetID() == Editor::MoveBtnID)
	{
		owner->gizmoMove = true;
		owner->UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::RotateBtnID)
	{
		owner->gizmoMove = false;
		owner->UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::GlobalBtnID)
	{
		owner->gizmoGlobal = true;
		owner->UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::LocalBtnID)
	{
		owner->gizmoGlobal = false;
		owner->UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::PlayBtnID)
	{
		owner->StartScene();
	}
}

void Editor::Listener::OnRightMouseUp(EUIWidget* sender, int mx, int my)
{
}

void Editor::Listener::OnMenuItem(EUIWidget* sender, int id)
{
	owner->ProcessMenu(id);
}

void Editor::Listener::OnUpdate(EUIWidget* sender)
{
	owner->Update();
}

void Editor::Listener::OnListBoxChange(EUIWidget* sender, int index)
{
	owner->SelectObject((SceneObject*)owner->sceneList->GetSelectedItemData());
}

void Editor::Listener::OnEditBoxChange(EUIWidget* sender)
{
	owner->OnObjectNameChanged();
}

void Editor::Listener::OnResize(EUIWidget* sender)
{
	if ((sender->GetID() == Editor::ViewportID && !owner->scene.Playing()) ||
		(sender->GetID() == Editor::GameViewportID && owner->scene.Playing()))
	{
		render.GetDevice()->SetVideoMode(sender->GetWidth(), sender->GetHeight(), sender->GetNative());
	}
}

void Editor::Listener::OnWinClose(EUIWidget* sender)
{
	if (owner->gameWnd && owner->gameWnd != sender)
	{
		owner->gameWnd->Close();
		owner->gameWnd = NULL;
	}

	owner->StopScene();
}

Editor::Editor() : listener(this)
{
	selectedObject = NULL;
	gizmoMove = true;
	gizmoGlobal = true;
	gameWnd = false;
}

Editor::~Editor()
{
}

void Editor::Init()
{
	EUI::Init("settings/EUI/theme.dat");

	mainWnd = new EUIWindow("Editor", false, true, 30, 30, 800, 600);
	mainWnd->SetListener(&listener, 0);

	EUIMenu* menu = new EUIMenu(mainWnd);
	menu->SetListener(&listener, 0);

	menu->StartSubMenu("File");

	menu->AddItem(MenuNewID, "New...");
	menu->AddItem(MenuOpenID, "Open...");
	menu->AddItem(MenuSaveID, "Save...");
	menu->AddItem(MenuSaveAsID, "Save as...");
	menu->AddSeparator();
	menu->AddItem(MenuExitID, "Exit...");

	menu->EndSubMenu();

	menu->StartSubMenu("Edit");

	menu->AddItem(MenuCopyID, "Copy");
	menu->AddItem(MenuDeleteID, "Delete");

	menu->EndSubMenu();

	menu->StartSubMenu("Create");

	ClassFactorySceneObject* decl = ClassFactorySceneObject::First();

	int id = MenuSceneObjectID;

	while (decl)
	{
		menu->AddItem(id, decl->GetName());
		id++;

		decl = decl->Next();
	}

	menu->EndSubMenu();

	menu->AddItem(1301, "About");

	EUILayout* lt = new EUILayout(mainWnd, false);

	EUIPanel* toolsPanel2 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel2, 200, false);

	EUILayout* leftPanelLt = new EUILayout(toolsPanel2, true);

	EUIPanel* toolsPanel = new EUIPanel(leftPanelLt, 10, 10, 100, 30);
	leftPanelLt->SetChildSize(toolsPanel, 30, false);

	moveBtn = new EUIButton(toolsPanel, "Move", 5, 5, 30, 20);
	moveBtn->SetPushable(true);
	moveBtn->SetID(MoveBtnID);
	moveBtn->SetListener(&listener, 0);

	rotateBtn = new EUIButton(toolsPanel, "Rot", 40, 5, 30, 20);
	rotateBtn->SetPushable(true);
	rotateBtn->SetID(RotateBtnID);
	rotateBtn->SetListener(&listener, 0);

	globalBtn = new EUIButton(toolsPanel, "Glob", 75, 5, 30, 20);
	globalBtn->SetPushable(true);
	globalBtn->SetID(GlobalBtnID);
	globalBtn->SetListener(&listener, 0);

	localBtn = new EUIButton(toolsPanel, "Loc", 110, 5, 30, 20);
	localBtn->SetPushable(true);
	localBtn->SetID(LocalBtnID);
	localBtn->SetListener(&listener, 0);

	playBtn = new EUIButton(toolsPanel, "Play", 165, 5, 30, 20);
	playBtn->SetID(PlayBtnID);
	playBtn->SetListener(&listener, 0);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(&listener, 0);

	EUITabSheet* sheet = tabPanel->AddTab("Scene");

	EUILayout* scene_lt = new EUILayout(sheet, false);

	sceneList = new EUIListBox(scene_lt, 200, 10, 200, 100);
	sceneList->SetListener(&listener, 0);

	sheet = tabPanel->AddTab("Object");

	EUILayout* object_lt = new EUILayout(sheet, true);

	objectName = new EUIEditBox(object_lt, "", 0, 0, 200, 20, EUIEditBox::InputText);
	objectName->SetListener(&listener, 0);
	objectName->Enable(false);
	object_lt->SetChildSize(objectName, 20, false);

	objCat = new EUICategories(object_lt, 0, 0, 100, 100);

	viewport = new EUIPanel(lt, 10, 10, 100, 30);
	viewport->SetListener(&listener, EUIWidget::OnResize | EUIWidget::OnUpdate);
	viewport->SetID(ViewportID);

	controls.Init(viewport->GetNative(), "settings/controls/hardware_pc", "settings/controls/user_pc");
	render.Init("DX11", 100, 100, viewport->GetNative());

	render.AddExecutedLevelPool(1);

	renderTaskPool = render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&Editor::Draw);

	Timer::Init();
	freecamera.Init();
	scene.Init();

	UpdateGizmoToolbar();

	mainWnd->Show(true);
	mainWnd->Maximaze();
}

int Editor::Run()
{
	prev_t = Timer::GetMillisFloat();

	return EUI::Run();
}

void Editor::ClearScene()
{
	SelectObject(NULL);
	sceneName.clear();
	sceneList->ClearList();
	scene.DeleteAllObjects();
}

void Editor::UpdateGizmoToolbar()
{
	gizmo.mode = gizmoMove ? 0 : 1;
	gizmo.useLocalSpace = !gizmoGlobal;
	moveBtn->SetPushed(gizmoMove);
	rotateBtn->SetPushed(!gizmoMove);
	globalBtn->SetPushed(gizmoGlobal);
	localBtn->SetPushed(!gizmoGlobal);
}

void Editor::SelectObject(SceneObject* obj)
{
	if (selectedObject)
	{
		selectedObject->GetMetaData()->HideWidgets();
	}

	selectedObject = obj;

	bool enabled = (selectedObject != NULL);
	objectName->Enable(enabled);
	gizmo.enabled = enabled;

	if (selectedObject)
	{
		objectName->SetText(obj->GetName());
		gizmo.transform = obj->Trans();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->PrepareWidgets(objCat);
		sceneList->SelectItemByData(obj);
	}
	else
	{
		objectName->SetText("");
	}
}

void Editor::CopyObject(SceneObject* obj)
{
	if (!obj)
	{
		return;
	}

	SceneObject* copy = scene.AddObject(obj->GetClassName());

	copy->Trans() = obj->Trans();
	copy->GetMetaData()->Copy(obj);
	obj->ApplyProperties();

	SetUniqueName(copy, obj->GetName());
	sceneList->AddItem(copy->GetName(), copy);
	sceneList->SelectItemByData(copy);

	SelectObject(copy);
}

void Editor::SetUniqueName(SceneObject* obj, const char* name)
{
	char baseName[512];
	int index = StringUtils::ExtractNameNumber(name, baseName, 512);
	bool unique = false;
	char uniqueName[512];

	while (!unique)
	{
		StringUtils::Printf(uniqueName, 512, "%s%i", baseName, index);

		index++;
		unique = (scene.Find(uniqueName) == NULL);
	}

	obj->SetName(uniqueName);
}

void Editor::OnObjectNameChanged()
{
	if (!selectedObject ||
		StringUtils::IsEqual(objectName->GetText(), selectedObject->GetName()))
	{
		return;
	}

	SetUniqueName(selectedObject, objectName->GetText());
	sceneList->ChangeItemNameByData(objectName->GetText(), selectedObject);
}

void Editor::CreateSceneObject(const char* name)
{
	SceneObject* obj = scene.AddObject(name);
	obj->ApplyProperties();

	obj->Trans().Move(freecamera.pos + Vector(cosf(freecamera.angles.x), sinf(freecamera.angles.y), sinf(freecamera.angles.x)) * 5.0f);

	SetUniqueName(obj, name);
	sceneList->AddItem(obj->GetName(), obj);
	sceneList->SelectItemByData(obj);

	SelectObject(obj);
}

void Editor::DeleteSceneObject(SceneObject* obj)
{
	if (!obj)
	{
		return;
	}

	if (selectedObject == obj)
	{
		SelectObject(NULL);
	}

	sceneList->DeleteItemByData(obj);
	scene.DeleteObject(obj);
}

void Editor::Update()
{
	float t = Timer::GetMillisFloat();
	float dt = (t - prev_t) * 0.001f;
	prev_t = t;

	if (!scene.Playing())
	{
		for (int i = 0; i <= 20; i++)
		{
			int pos = i - 10;

			render.DebugLine(Vector(pos, 0, -10), COLOR_WHITE, Vector(pos, 0, 10), COLOR_WHITE);
			render.DebugLine(Vector(-10, 0, pos), COLOR_WHITE, Vector(10, 0, pos), COLOR_WHITE);
		}

		gizmo.Render();

		freecamera.Update(dt);

		if (selectedObject && selectedObject->GetMetaData()->IsValueWasChanged())
		{
			selectedObject->ApplyProperties();
		}
	}

	scene.Execute(dt);

	render.Execute(dt);

	controls.Update(dt);
}

void Editor::StartScene()
{
	scene.Play();

	gameWnd = new EUIWindow("Game", false, true, 0, 0, 800, 600);
	gameWnd->SetListener(&listener, 0);
	
	EUIPanel* pn = new EUIPanel(gameWnd, 0, 0, 800, 600);
	pn->SetListener(&listener, EUIWidget::OnResize | EUIWidget::OnUpdate);
	pn->SetID(GameViewportID);

	gameWnd->Show(true);
	gameWnd->SetAtScreenCenter();
}

void Editor::StopScene()
{
	scene.Stop();

	render.GetDevice()->SetVideoMode(viewport->GetWidth(), viewport->GetHeight(), viewport->GetNative());

	gameWnd = NULL;
}

void Editor::Draw(float dt)
{
	render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

	render.ExecutePool(0, dt);
	render.ExecutePool(1000, dt);

	render.GetDevice()->Present();
}

void Editor::ProcessMenu(int activated_id)
{
	ClassFactorySceneObject* decl = ClassFactorySceneObject::First();

	if (activated_id == MenuCopyID)
	{
		CopyObject(selectedObject);
	}

	if (activated_id == MenuDeleteID)
	{
		DeleteSceneObject(selectedObject);
	}

	if (activated_id == MenuNewID)
	{
		ClearScene();
	}

	if (activated_id == MenuSaveID && sceneName.size() != 0)
	{
		scene.Save(sceneName.c_str());
	}

	if ((activated_id == MenuSaveID && sceneName.size() == 0) ||
		activated_id == MenuSaveAsID)
	{
		const char* fileName = EUI::EUI::OpenSaveDialog("Scene file", "scn");

		if (fileName)
		{
			sceneName = fileName;
		}

		scene.Save(sceneName.c_str());
	}

	if (activated_id == MenuOpenID)
	{
		const char* fileName = EUI::OpenOpenDialog("Scene file", "scn");

		if (fileName)
		{
			ClearScene();
			sceneName = fileName;
			scene.Load(fileName);

			for (int i = 0;i<scene.GetObjectsCount(); i++)
			{
				SceneObject* obj = scene.GetObj(i);
				sceneList->AddItem(obj->GetName(), obj);
			}
		}
	}

	if (activated_id >= MenuSceneObjectID && activated_id <= MenuSceneObjectID + 100)
	{
		int id = MenuSceneObjectID;

		while (decl)
		{
			if (id == activated_id)
			{
				CreateSceneObject(decl->GetName());
				break;
			}

			id++;
			decl = decl->Next();
		}
	}

	if (activated_id == MenuExitID)
	{
		mainWnd->Close();

		if (gameWnd)
		{
			gameWnd->Close();
		}
	}
}