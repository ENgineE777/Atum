
#include "Editor.h"
#include "SceneAssets/Sprite.h"
#include "SceneObjects/RenderLevels.h"

char appdir[1024];

Editor::Editor()
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

	mainWnd = new EUIWindow("Editor", EUIWindow::Normal, true, 30, 30, 800, 600);
	mainWnd->SetListener(-1, this, 0);

	EUIMenu* menu = new EUIMenu(mainWnd);
	menu->SetListener(-1, this, 0);

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

	vector<const char*> names;

	while (decl)
	{
		names.push_back(decl->GetName());

		for (int i = (int)names.size() - 1; i > 0; i--)
		{
			if (StringUtils::CompareABC(names[i], names[i - 1]))
			{
				const char* tmp = names[i - 1];
				names[i - 1] = names[i];
				names[i] = tmp;
			}
		}

		decl = decl->Next();
	}

	for (auto name : names)
	{
		menu->AddItem(id, name);
		id2name[id] = name;
		id++;
	}

	menu->EndSubMenu();

	menu->StartSubMenu("Asset");

	ClassFactorySceneAsset* declAsset = ClassFactorySceneAsset::First();

	id = MenuSceneAssetID;

	names.clear();

	while (declAsset)
	{
		names.push_back(declAsset->GetName());

		for (int i = (int)names.size() - 1; i > 0; i--)
		{
			if (StringUtils::CompareABC(names[i], names[i - 1]))
			{
				const char* tmp = names[i - 1];
				names[i - 1] = names[i];
				names[i] = tmp;
			}
		}

		declAsset = declAsset->Next();
	}

	for (auto name : names)
	{
		menu->AddItem(id, name);
		id2name[id] = name;
		id++;
	}

	menu->EndSubMenu();

	menu->EndSubMenu();

	menu->AddItem(1301, "About");

	EUILayout* lt = new EUILayout(mainWnd, false);

	EUIPanel* toolsPanel2 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel2, 250, false);

	EUILayout* leftPanelLt = new EUILayout(toolsPanel2, true);

	EUIPanel* toolsPanel = new EUIPanel(leftPanelLt, 10, 10, 100, 30);
	leftPanelLt->SetChildSize(toolsPanel, 30, false);

	moveBtn = new EUIButton(toolsPanel, "Move", 5, 5, 30, 20);
	moveBtn->SetPushable(true);
	moveBtn->SetListener(MoveBtnID, this, 0);

	rotateBtn = new EUIButton(toolsPanel, "Rot", 40, 5, 30, 20);
	rotateBtn->SetPushable(true);
	rotateBtn->SetListener(RotateBtnID, this, 0);

	globalBtn = new EUIButton(toolsPanel, "Glob", 75, 5, 30, 20);
	globalBtn->SetPushable(true);
	globalBtn->SetListener(GlobalBtnID, this, 0);

	localBtn = new EUIButton(toolsPanel, "Loc", 110, 5, 30, 20);
	localBtn->SetPushable(true);
	localBtn->SetListener(LocalBtnID, this, 0);

	mode2DBtn = new EUIButton(toolsPanel, "2D", 145, 5, 30, 20);
	mode2DBtn->SetPushable(true);
	mode2DBtn->SetListener(-1, this, 0);

	playBtn = new EUIButton(toolsPanel, "Play", 215, 5, 30, 20);
	playBtn->SetListener(PlayBtnID, this, 0);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(-1, this, 0);

	EUITabSheet* sheet = tabPanel->AddTab("Scene");

	EUILayout* scene_lt = new EUILayout(sheet, true);

	sceneList = new EUIListBox(scene_lt, 200, 10, 200, 100, true);
	scene_lt->SetChildSize(sceneList, 0.5, true);
	sceneList->SetListener(SceneListID, this, 0);

	assetList = new EUIListBox(scene_lt, 200, 10, 200, 100, true);
	assetList->SetListener(AssetListID, this, 0);

	EUITabPanel* viewportPanels = new EUITabPanel(lt, 30, 50, 100, 30);
	viewportPanels->SetListener(-1, this, 0);

	sheet = viewportPanels->AddTab("Scene");
	viewport_lt = new EUILayout(sheet, true);

	viewport = new EUIPanel(viewport_lt, 10, 10, 100, 30);
	viewport->SetListener(ViewportID, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	asset_vp_sheet_lt = new EUILayout(sheet, false);

	EUITabPanel* tabPanelB = new EUITabPanel(asset_vp_sheet_lt, 30, 50, 100, 30);
	asset_vp_sheet_lt->SetChildSize(tabPanelB, 200, false);

	asset_viewport = new EUIPanel(asset_vp_sheet_lt, 10, 10, 100, 30);
	asset_viewport->SetListener(AssetViewportID, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	EUIPanel* toolsPanel3 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel3, 200, false);

	EUILayout* object_lt = new EUILayout(toolsPanel3, true);

	objectName = new EUIEditBox(object_lt, "", 0, 0, 200, 20, EUIEditBox::InputText);
	objectName->SetListener(-1, this, 0);
	objectName->Enable(false);
	object_lt->SetChildSize(objectName, 20, false);

	objCat = new EUICategories(object_lt, 0, 0, 100, 100);

	core.Init(viewport->GetNative());

	render.AddExecutedLevelPool(1);

	renderTaskPool = render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&Editor::Draw);

	freecamera.Init();
	ed_scene.Init();

	gizmo.Init();

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
	ed_scene.DeleteAllObjects();
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

	SceneObject* copy = ed_scene.AddObject(obj->GetClassName());

	copy->Copy(obj);

	SetUniqueName(copy, obj->GetName());
	sceneList->AddItem(copy->GetName(), copy);
	sceneList->SelectItemByData(copy);

	SelectObject(copy);
}

void Editor::SetUniqueName(SceneObject* obj, const char* name)
{
	if (!ed_scene.Find(name))
	{
		obj->SetName(name);
		return;
	}

	char baseName[512];
	int index = StringUtils::ExtractNameNumber(name, baseName, 512);
	bool unique = false;
	char uniqueName[512];

	while (!unique)
	{
		StringUtils::Printf(uniqueName, 512, "%s%i", baseName, index);
		index++;
		unique = (ed_scene.Find(uniqueName) == NULL);
	}

	obj->SetName(uniqueName);
	objectName->SetText(uniqueName);
}

void Editor::CreateSceneObject(const char* name)
{
	SceneObject* obj = ed_scene.AddObject(name);
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
	ed_scene.DeleteObject(obj);
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

	SceneAsset* copy = ed_scene.AddAsset(obj->GetClassName());

	copy->Copy(obj);

	SetUniqueAssetName(copy, obj->GetName());
	assetList->AddItem(copy->GetName(), copy);
	assetList->SelectItemByData(copy);

	SelectAsset(copy);
}

void Editor::SetUniqueAssetName(SceneAsset* obj, const char* name)
{
	if (!ed_scene.FindAsset(name))
	{
		obj->SetName(name);
		return;
	}

	char baseName[512];
	int index = StringUtils::ExtractNameNumber(name, baseName, 512);
	bool unique = false;
	char uniqueName[512];

	while (!unique)
	{
		StringUtils::Printf(uniqueName, 512, "%s%i", baseName, index);

		index++;
		unique = (ed_scene.FindAsset(uniqueName) == NULL);
	}

	obj->SetName(uniqueName);
	objectName->SetText(uniqueName);
}

void Editor::ShowVieport()
{
	EUIPanel* vp = selectedAsset ? asset_viewport : viewport;

	if (scene)
	{
		vp = game_viewport;
	}
	else
	{
		viewport_lt->Show(selectedObject || !selectedAsset);
		asset_vp_sheet_lt->Show(selectedAsset != nullptr);
		ed_scene.EnableTasks(selectedAsset == nullptr);
	}

	render.GetDevice()->SetVideoMode((int)vp->GetWidth(), (int)vp->GetHeight(), vp->GetNative());
	controls.SetWindow(vp->GetNative());
}

void Editor::CreateSceneAsset(const char* name)
{
	SceneAsset* obj = ed_scene.AddAsset(name);
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
	ed_scene.DeleteAsset(obj);
}

void Editor::StartScene()
{
	if (selectedAsset)
	{
		selectedAsset->EnableTasks(false);
	}
	else
	{
		ed_scene.EnableTasks(false);
	}

	ed_scene.Save(sceneName.c_str());

	scene = new Scene();
	scene->Init();
	scene->Load(sceneName.c_str());
	scene->Play();

	gameWnd = new EUIWindow("Game", EUIWindow::PopupWithCloseBtn, true, 0, 0, 800, 600);
	gameWnd->SetListener(-1, this, 0);
	
	EUILayout* lt = new EUILayout(gameWnd, false);

	game_viewport = new EUIPanel(lt, 0, 0, 800, 600);
	game_viewport->SetListener(GameViewportID, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	controls.SetWindow(game_viewport->GetNative());
	game_viewport->SetFocused();

	gameWnd->Show(true);
	gameWnd->SetAtScreenCenter();
}

void Editor::StopScene()
{
	if (!scene)
	{
		return;
	}

	scene->Stop();
	RELEASE(scene);

	if (selectedAsset)
	{
		selectedAsset->EnableTasks(true);
	}
	else
	{
		ed_scene.EnableTasks(true);
	}

	ShowVieport();

	gameWnd = nullptr;
	game_viewport = nullptr;
}

void Editor::Draw(float dt)
{
	render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

	render.ExecutePool(RenderLevels::Camera, dt);
	render.ExecutePool(RenderLevels::Prepare, dt);
	render.ExecutePool(RenderLevels::Geometry, dt);
	render.ExecutePool(RenderLevels::DebugGeometry, dt);
	render.ExecutePool(RenderLevels::Sprites, dt);
	render.ExecutePool(RenderLevels::PostProcess, dt);
	render.ExecutePool(RenderLevels::GUI, dt);
	render.ExecutePool(RenderLevels::Debug, dt);

	render.GetDevice()->Present();
}

void Editor::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		controls.OverrideMousePos(mx, my);
	}

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		Vector2 ms((float)mx, (float)my);

		if (controls.DebugKeyPressed("MS_BTN1", Controls::Active) && controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
		{
			Sprite::ed_cam_pos -= ms - prev_ms;
		}

		gizmo.OnMouseMove((float)mx, (float)my);

		if (selectedObject)
		{
			selectedObject->OnMouseMove(ms - prev_ms);
		}

		if (selectedAsset)
		{
			selectedAsset->OnMouseMove(ms - prev_ms);
		}

		if (selectedObject && !scene)
		{
			if (allowCopy && controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active))
			{
				if (!selectedObject->Trans().IsEqual(gizmo.transform))
				{
					allowCopy = false;
					CopyObject(selectedObject);
				}
			}

			selectedObject->Trans() = gizmo.transform;
		}
	}

	prev_ms = Vector2((float)mx, (float)my);
}

void Editor::OnLeftMouseDown(EUIWidget* sender, int mx, int my)
{
	prev_ms = Vector2((float)mx, (float)my);

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		allowCopy = true;
		gizmo.OnLeftMouseDown((float)mx, (float)my);
		sender->CaptureMouse();

		if (selectedObject)
		{
			selectedObject->OnLeftMouseDown(prev_ms);
		}

		if (selectedAsset)
		{
			selectedAsset->OnLeftMouseDown(prev_ms);
		}
	}

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		sender->SetFocused();
	}
}

void Editor::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID ||
		sender->GetID() == Editor::GameViewportID)
	{
		gizmo.OnLeftMouseUp();
		sender->ReleaseMouse();

		if (selectedObject)
		{
			selectedObject->OnLeftMouseUp();
		}

		if (selectedAsset)
		{
			selectedAsset->OnLeftMouseUp();
		}
	}

	if (sender->GetID() == Editor::MoveBtnID)
	{
		gizmoMove = true;
		UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::RotateBtnID)
	{
		gizmoMove = false;
		UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::GlobalBtnID)
	{
		gizmoGlobal = true;
		UpdateGizmoToolbar();
	}

	if (sender->GetID() == Editor::LocalBtnID)
	{
		gizmoGlobal = false;
		UpdateGizmoToolbar();
	}

	if (sender == mode2DBtn)
	{
		freecamera.mode_2d = !freecamera.mode_2d;
	}

	if (sender->GetID() == Editor::PlayBtnID)
	{
		StartScene();
	}
}

void Editor::OnRightMouseDown(EUIWidget* sender, int mx, int my)
{
	prev_ms = Vector2((float)mx, (float)my);

	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID)
	{
		sender->CaptureMouse();

		if (selectedObject)
		{
			selectedObject->OnRightMouseDown(prev_ms);
		}

		if (selectedAsset)
		{
			selectedAsset->OnRightMouseDown(prev_ms);
		}
	}
}

void Editor::OnRightMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == Editor::ViewportID ||
		sender->GetID() == Editor::AssetViewportID)
	{
		sender->ReleaseMouse();

		if (selectedObject)
		{
			selectedObject->OnRightMouseUp();
		}

		if (selectedAsset)
		{
			selectedAsset->OnRightMouseUp();
		}
	}
}

void Editor::OnMenuItem(EUIWidget* sender, int activated_id)
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
		ed_scene.Save(sceneName.c_str());
	}

	if ((activated_id == MenuSaveID && sceneName.size() == 0) ||
		activated_id == MenuSaveAsID)
	{
		const char* fileName = EUI::EUI::OpenSaveDialog(mainWnd->GetNative(), "Scene file", "scn");

		if (fileName)
		{
			sceneName = fileName;
		}

		ed_scene.Save(sceneName.c_str());
	}

	if (activated_id == MenuOpenID)
	{
		const char* fileName = EUI::OpenOpenDialog(mainWnd->GetNative(), "Scene file", "scn");

		if (fileName)
		{
			ClearScene();
			sceneName = fileName;
			ed_scene.Load(fileName);

			for (int i = 0; i<ed_scene.GetObjectsCount(); i++)
			{
				SceneObject* obj = ed_scene.GetObj(i);
				sceneList->AddItem(obj->GetName(), obj);
			}

			for (int i = 0; i<ed_scene.GetAssetsCount(); i++)
			{
				SceneAsset* obj = ed_scene.GetAsset(i);
				assetList->AddItem(obj->GetName(), obj);
			}
		}
	}

	if (activated_id >= MenuSceneObjectID && activated_id <= MenuSceneObjectID + 1000)
	{
		CreateSceneObject(id2name[activated_id].c_str());
	}

	if (activated_id >= MenuSceneAssetID && activated_id <= MenuSceneAssetID + 100)
	{
		CreateSceneAsset(id2name[activated_id].c_str());
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

void Editor::OnUpdate(EUIWidget* sender)
{
	core.CountDeltaTime();
	float dt = core.GetDeltaTime();

	if (!scene)
	{
		if (!freecamera.mode_2d)
		{
			for (int i = 0; i <= 20; i++)
			{
				float pos = (float)i - 10.0f;

				render.DebugLine(Vector(pos, 0.0f, -10.0f), COLOR_WHITE, Vector(pos, 0.0f, 10.0f), COLOR_WHITE);
				render.DebugLine(Vector(-10.0f, 0.0f, pos), COLOR_WHITE, Vector(10.0f, 0.0f, pos), COLOR_WHITE);
			}
		}

		freecamera.Update(dt);

		gizmo.Render();

		if (selectedObject && selectedObject->GetMetaData()->IsValueWasChanged())
		{
			selectedObject->ApplyProperties();
		}

		if (selectedAsset && selectedAsset->GetMetaData()->IsValueWasChanged())
		{
			selectedAsset->ApplyProperties();
		}
	}

	if (viewport->IsFocused() || asset_viewport->IsFocused())
	{
		Color color = COLOR_RED;

		for (float i = 0; i < 3.0f; i+=1.0f)
		{
			render.DebugLine2D(Vector2(1.0f, 1.0f + i), color, Vector2((float)render.GetDevice()->GetWidth(), 1.0f + i), color);
			render.DebugLine2D(Vector2(1.0f, (float)render.GetDevice()->GetHeight() - 1.0f - i), color, Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight() - 1.0f - i), color);
			render.DebugLine2D(Vector2(1.0f + i, 1.0f), color, Vector2(1.0f + i, (float)render.GetDevice()->GetHeight()), color);
			render.DebugLine2D(Vector2((float)render.GetDevice()->GetWidth() - 1.0f - i, 1.0f), color, Vector2((float)render.GetDevice()->GetWidth() - 1.0f - i, (float)render.GetDevice()->GetHeight()), color);
		}
	}

	if (scene)
	{
		scene->Execute(dt);
	}
	else
	{
		if (selectedAsset)
		{
			selectedAsset->Tasks()->Execute(dt);
		}
		else
		{
			ed_scene.Execute(dt);
		}
	}

	render.DebugPrintText(5.0f, COLOR_WHITE, "%i",core.GetFPS());

	core.Update();
}

void Editor::OnListBoxChange(EUIWidget* sender, int index)
{
	if (sender->GetID() == SceneListID)
	{
		SelectObject((SceneObject*)sceneList->GetSelectedItemData());
	}
	else
	if (sender->GetID() == AssetListID)
	{
		SelectAsset((SceneAsset*)assetList->GetSelectedItemData());
	}
}

void Editor::OnEditBoxStopEditing(EUIWidget* sender)
{
	if (sender == objectName)
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
}

void Editor::OnResize(EUIWidget* sender)
{
	if ((sender->GetID() == Editor::ViewportID && !scene) ||
		(sender->GetID() == Editor::AssetViewportID && !scene) ||
		(sender->GetID() == Editor::GameViewportID && scene))
	{
		ShowVieport();
	}
}

void Editor::OnWinClose(EUIWidget* sender)
{
	if (gameWnd && gameWnd != sender)
	{
		gameWnd->Close();
		gameWnd = nullptr;
	}

	StopScene();
}