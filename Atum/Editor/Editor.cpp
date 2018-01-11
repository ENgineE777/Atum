
#include "Editor.h"
#include "Services/Physics/Physics.h"
#include "Services/Font/FontService.h"

char appdir[1024];

Editor::Listener::Listener(Editor* set_owner)
{
	owner = set_owner;
}

void Editor::Listener::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		controls.OverrideMousePos(mx, my);
	}

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID)
	{
		owner->gizmo.OnMouseMove((float)mx / (float)sender->GetWidth(),
		                         (float)my / (float)sender->GetHeight());


		if (owner->selectedObject && !owner->scene.Playing())
		{
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
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID)
	{
		allowCopy = true;
		owner->gizmo.OnLeftMouseDown((float)mx / (float)sender->GetWidth(),
		                             (float)my / (float)sender->GetHeight());
	}

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		SetFocus(*(HWND*)sender->GetNative());
	}
}

void Editor::Listener::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID)
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
	if (sender->GetID() == SceneListID)
	{
		owner->SelectObject((SceneObject*)owner->sceneList->GetSelectedItemData());
	}
	else
	if (sender->GetID() == AssetListID)
	{
		owner->SelectAsset((SceneAsset*)owner->assetList->GetSelectedItemData());
	}
}

void Editor::Listener::OnEditBoxChange(EUIWidget* sender)
{
	owner->OnObjectNameChanged();
}

void Editor::Listener::OnResize(EUIWidget* sender)
{
	if ((sender->GetID() == Editor::ViewportID && !owner->scene.Playing()) ||
		(sender->GetID() == Editor::AssetViewportID && !owner->scene.Playing()) ||
		(sender->GetID() == Editor::GameViewportID && owner->scene.Playing()))
	{
		render.GetDevice()->SetVideoMode((int)sender->GetWidth(), (int)sender->GetHeight(), sender->GetNative());
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
	selectedAsset = nullptr;
	gizmoMove = true;
	gizmoGlobal = true;
	gameWnd = false;
	Gizmo::inst = &gizmo;
}

Editor::~Editor()
{
}

void Editor::Init()
{
	GetCurrentDirectory(1024, appdir);

	EUI::Init("settings/EUI/theme.dat");

	mainWnd = new EUIWindow("Editor", false, true, 30, 30, 800, 600);
	mainWnd->SetListener(-1, &listener, 0);

	EUIMenu* menu = new EUIMenu(mainWnd);
	menu->SetListener(-1, &listener, 0);

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

	menu->StartSubMenu("Object");

	ClassFactorySceneObject* decl = ClassFactorySceneObject::First();

	int id = MenuSceneObjectID;

	while (decl)
	{
		menu->AddItem(id, decl->GetName());
		id++;

		decl = decl->Next();
	}

	menu->EndSubMenu();

	menu->StartSubMenu("Asset");

	ClassFactorySceneAsset* declAsset = ClassFactorySceneAsset::First();

	id = MenuSceneAssetID;

	while (declAsset)
	{
		menu->AddItem(id, declAsset->GetName());
		id++;

		declAsset = declAsset->Next();
	}

	menu->EndSubMenu();

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
	moveBtn->SetListener(MoveBtnID, &listener, 0);

	rotateBtn = new EUIButton(toolsPanel, "Rot", 40, 5, 30, 20);
	rotateBtn->SetPushable(true);
	rotateBtn->SetListener(RotateBtnID, &listener, 0);

	globalBtn = new EUIButton(toolsPanel, "Glob", 75, 5, 30, 20);
	globalBtn->SetPushable(true);
	globalBtn->SetListener(GlobalBtnID, &listener, 0);

	localBtn = new EUIButton(toolsPanel, "Loc", 110, 5, 30, 20);
	localBtn->SetPushable(true);
	localBtn->SetListener(LocalBtnID, &listener, 0);

	playBtn = new EUIButton(toolsPanel, "Play", 165, 5, 30, 20);
	playBtn->SetListener(PlayBtnID, &listener, 0);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(-1, &listener, 0);

	EUITabSheet* sheet = tabPanel->AddTab("Scene");

	EUILayout* scene_lt = new EUILayout(sheet, true);

	sceneList = new EUIListBox(scene_lt, 200, 10, 200, 100);
	scene_lt->SetChildSize(sceneList, 0.5, true);
	sceneList->SetListener(SceneListID, &listener, 0);

	assetList = new EUIListBox(scene_lt, 200, 10, 200, 100);
	assetList->SetListener(AssetListID, &listener, 0);

	EUITabPanel* viewportPanels = new EUITabPanel(lt, 30, 50, 100, 30);
	viewportPanels->SetListener(-1, &listener, 0);

	sheet = viewportPanels->AddTab("Scene");
	viewport_lt = new EUILayout(sheet, true);

	viewport = new EUIPanel(viewport_lt, 10, 10, 100, 30);
	viewport->SetListener(ViewportID, &listener, EUIWidget::OnResize | EUIWidget::OnUpdate);

	asset_vp_sheet_lt = new EUILayout(sheet, false);

	EUITabPanel* tabPanelB = new EUITabPanel(asset_vp_sheet_lt, 30, 50, 100, 30);
	asset_vp_sheet_lt->SetChildSize(tabPanelB, 200, false);

	asset_viewport = new EUIPanel(asset_vp_sheet_lt, 10, 10, 100, 30);
	asset_viewport->SetListener(AssetViewportID, &listener, EUIWidget::OnResize | EUIWidget::OnUpdate);

	EUIPanel* toolsPanel3 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel3, 200, false);

	EUILayout* object_lt = new EUILayout(toolsPanel3, true);

	objectName = new EUIEditBox(object_lt, "", 0, 0, 200, 20, EUIEditBox::InputText);
	objectName->SetListener(-1, &listener, 0);
	objectName->Enable(false);
	object_lt->SetChildSize(objectName, 20, false);

	objCat = new EUICategories(object_lt, 0, 0, 100, 100);

	controls.Init("settings/controls/hardware_pc", true);
	controls.LoadAliases("settings/controls/user_pc");
	controls.SetWindow(viewport->GetNative());

	fonts.Init();

	render.Init("DX11", 100, 100, viewport->GetNative());

	render.AddExecutedLevelPool(1);

	renderTaskPool = render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&Editor::Draw);

	physics.Init();

	freecamera.Init();
	scene.Init();

	UpdateGizmoToolbar();

	mainWnd->Show(true);
	mainWnd->Maximaze();

	ShowVieport();
}

int Editor::Run()
{
	return EUI::Run();
}

void Editor::ClearScene()
{
	SelectObject(NULL);
	sceneName.clear();
	sceneList->ClearList();
	assetList->ClearList();
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
	if (selectedAsset)
	{
		selectedAsset->GetMetaData()->HideWidgets();
		selectedAsset->EnableTasks(false);
		selectedAsset->SetEditMode(false);
		assetList->SelectItemByData(NULL);
		selectedAsset = NULL;
	}

	if (selectedObject)
	{
		selectedObject->SetEditMode(false);
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
		obj->SetEditMode(true);
		sceneList->SelectItemByData(obj);
	}
	else
	{
		objectName->SetText("");
		sceneList->SelectItemByData(NULL);
	}

	ShowVieport();
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
	if (selectedObject)
	{
		if (StringUtils::IsEqual(objectName->GetText(), selectedObject->GetName()))
		{
			return;
		}

		SetUniqueName(selectedObject, objectName->GetText());
		sceneList->ChangeItemNameByData(objectName->GetText(), selectedObject);
	}

	if (selectedAsset)
	{
		if (StringUtils::IsEqual(objectName->GetText(), selectedAsset->GetName()))
		{
			return;
		}

		SetUniqueAssetName(selectedAsset, objectName->GetText());
		assetList->ChangeItemNameByData(objectName->GetText(), selectedAsset);
	}
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

void Editor::SelectAsset(SceneAsset* obj)
{
	if (selectedObject)
	{
		selectedObject->GetMetaData()->HideWidgets();
		selectedObject->SetEditMode(true);
		sceneList->SelectItemByData(NULL);
		selectedObject = NULL;
	}

	if (selectedAsset)
	{
		selectedAsset->GetMetaData()->HideWidgets();
		selectedAsset->EnableTasks(false);
		selectedAsset->SetEditMode(false);
	}

	selectedAsset = obj;

	bool enabled = (selectedAsset != NULL);
	objectName->Enable(enabled);
	gizmo.enabled = enabled;

	if (selectedAsset)
	{
		objectName->SetText(obj->GetName());
		gizmo.transform = obj->Trans();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->PrepareWidgets(objCat);
		obj->SetEditMode(true);
		assetList->SelectItemByData(obj);

		selectedAsset->EnableTasks(true);
	}
	else
	{
		objectName->SetText("");
	}

	ShowVieport();
}

void Editor::CopyAsset(SceneAsset* obj)
{
	if (!obj)
	{
		return;
	}

	SceneAsset* copy = scene.AddAsset(obj->GetClassName());

	copy->Trans() = obj->Trans();
	copy->GetMetaData()->Copy(obj);
	obj->ApplyProperties();

	SetUniqueAssetName(copy, obj->GetName());
	assetList->AddItem(copy->GetName(), copy);
	assetList->SelectItemByData(copy);

	SelectAsset(copy);
}

void Editor::SetUniqueAssetName(SceneAsset* obj, const char* name)
{
	char baseName[512];
	int index = StringUtils::ExtractNameNumber(name, baseName, 512);
	bool unique = false;
	char uniqueName[512];

	while (!unique)
	{
		StringUtils::Printf(uniqueName, 512, "%s%i", baseName, index);

		index++;
		unique = (scene.FindAsset(uniqueName) == NULL);
	}

	obj->SetName(uniqueName);
}

void Editor::ShowVieport()
{
	viewport_lt->Show(selectedObject || !selectedAsset);
	asset_vp_sheet_lt->Show(selectedAsset != nullptr);

	EUIPanel* vp = selectedAsset ? asset_viewport : viewport;

	render.GetDevice()->SetVideoMode((int)viewport->GetWidth(), (int)viewport->GetHeight(), vp->GetNative());

	controls.SetWindow(vp->GetNative());

	scene.EnableTasks(selectedAsset == nullptr);
}

void Editor::CreateSceneAsset(const char* name)
{
	SceneAsset* obj = scene.AddAsset(name);
	obj->ApplyProperties();

	obj->Trans().Move(freecamera.pos + Vector(cosf(freecamera.angles.x), sinf(freecamera.angles.y), sinf(freecamera.angles.x)) * 5.0f);

	SetUniqueAssetName(obj, name);
	assetList->AddItem(obj->GetName(), obj);
	assetList->SelectItemByData(obj);

	SelectAsset(obj);
}

void Editor::DeleteSceneAsset(SceneAsset* obj)
{
	if (!obj)
	{
		return;
	}

	if (selectedAsset == obj)
	{
		SelectAsset(NULL);
	}

	assetList->DeleteItemByData(obj);
	scene.DeleteAsset(obj);
}

void Editor::Update()
{
	float dt = Timer::CountDeltaTime();

	if (!scene.Playing())
	{
		for (int i = 0; i <= 20; i++)
		{
			float pos = (float)i - 10.0f;

			render.DebugLine(Vector(pos, 0.0f, -10.0f), COLOR_WHITE, Vector(pos, 0.0f, 10.0f), COLOR_WHITE);
			render.DebugLine(Vector(-10.0f, 0.0f, pos), COLOR_WHITE, Vector(10.0f, 0.0f, pos), COLOR_WHITE);
		}

		gizmo.Render();

		freecamera.Update(dt);

		if (selectedObject && selectedObject->GetMetaData()->IsValueWasChanged())
		{
			selectedObject->ApplyProperties();
		}

		if (selectedAsset && selectedAsset->GetMetaData()->IsValueWasChanged())
		{
			selectedAsset->ApplyProperties();
		}
	}

	char fps_str[16];
	StringUtils::Printf(fps_str, 16, "%i ", Timer::GetFPS());

	if (GetFocus() == *(HWND*)viewport->GetNative() ||
		GetFocus() == *(HWND*)asset_viewport->GetNative())
	{
		StringUtils::Cat(fps_str, 16, " Active");
	}

	render.DebugPrintText(0.0f, COLOR_GREEN, fps_str);

	physics.Update(dt);

	if (selectedAsset)
	{
		selectedAsset->Tasks()->Execute(dt);
	}
	else
	{
		scene.Execute(dt);
	}

	render.Execute(dt);

	controls.Update(dt);

	physics.Fetch();
}

void Editor::StartScene()
{
	scene.Play();

	gameWnd = new EUIWindow("Game", false, true, 0, 0, 800, 600);
	gameWnd->SetListener(-1, &listener, 0);
	
	EUILayout* lt = new EUILayout(gameWnd, false);

	EUIPanel* pn = new EUIPanel(lt, 0, 0, 800, 600);
	pn->SetListener(GameViewportID, &listener, EUIWidget::OnResize | EUIWidget::OnUpdate);

	controls.SetWindow(pn->GetNative());
	SetFocus(*(HWND*)pn->GetNative());

	gameWnd->Show(true);
	gameWnd->SetAtScreenCenter();
}

void Editor::StopScene()
{
	scene.Stop();

	ShowVieport();

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
	if (activated_id == MenuCopyID)
	{
		if (selectedObject)
		{
			CopyObject(selectedObject);
		}
		else
		if (selectedAsset)
		{
			CopyAsset(selectedAsset);
		}
	}

	if (activated_id == MenuDeleteID)
	{
		if (selectedObject)
		{
			DeleteSceneObject(selectedObject);
		}
		else
		if (selectedAsset)
		{
			DeleteSceneAsset(selectedAsset);
		}
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
		const char* fileName = EUI::EUI::OpenSaveDialog(mainWnd->GetNative(), "Scene file", "scn");

		if (fileName)
		{
			sceneName = fileName;
		}

		scene.Save(sceneName.c_str());
	}

	if (activated_id == MenuOpenID)
	{
		const char* fileName = EUI::OpenOpenDialog(mainWnd->GetNative(), "Scene file", "scn");

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

			for (int i = 0; i<scene.GetAssetsCount(); i++)
			{
				SceneAsset* obj = scene.GetAsset(i);
				assetList->AddItem(obj->GetName(), obj);
			}
		}
	}

	if (activated_id >= MenuSceneObjectID && activated_id <= MenuSceneObjectID + 1000)
	{
		int id = MenuSceneObjectID;

		ClassFactorySceneObject* decl = ClassFactorySceneObject::First();

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

	if (activated_id >= MenuSceneAssetID && activated_id <= MenuSceneAssetID + 100)
	{
		int id = MenuSceneAssetID;

		ClassFactorySceneAsset* decl = ClassFactorySceneAsset::First();

		while (decl)
		{
			if (id == activated_id)
			{
				CreateSceneAsset(decl->GetName());
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