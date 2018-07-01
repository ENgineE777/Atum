
#include "Editor.h"
#include "SceneAssets/Sprite.h"
#include "SceneObjects/RenderLevels.h"

char appdir[1024];

EUITabPanel* Editor::outputPanels = nullptr;
map<string, EUIListBox*> Editor::output_boxes;

void Editor::Init()
{
	GetCurrentDirectory(1024, appdir);

	EUI::Init("settings/EUI/theme.dat");

	mainWnd = new EUIWindow("Editor", EUIWindow::Normal, true, 30, 30, 800, 600);
	mainWnd->SetListener(-1, this, 0);

	popup_menu = new EUIMenu();
	popup_menu->SetListener(-1, this, 0);

	EUIMenu* menu = new EUIMenu();
	menu->SetListener(-1, this, 0);

	menu->StartMenu(false);
	menu->StartSubMenu("File");

	menu->AddItem(MenuNewID, "New...");
	menu->AddItem(MenuOpenID, "Open...");
	menu->AddItem(MenuSaveID, "Save...");
	menu->AddItem(MenuSaveAsID, "Save as...");
	menu->AddSeparator();
	menu->AddItem(MenuExitID, "Exit...");

	menu->EndSubMenu();

	menu->EndSubMenu();

	menu->AddItem(1301, "About");

	menu->AttachToWidget(mainWnd);

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

	playBtn = new EUIButton(toolsPanel, "Play", 215, 5, 30, 20);
	playBtn->SetListener(PlayBtnID, this, 0);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(-1, this, 0);

	EUITabSheet* sheet = tabPanel->AddTab("Scene");

	EUILayout* scene_lt = new EUILayout(sheet, true);

	scene_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	scene_lt->SetChildSize(scene_treeview, 0.5, true);
	scene_treeview->SetListener(SceneListID, this, 0);

	scene_treeview->AddImage("settings/editor/folder.bmp");
	scene_treeview->AddImage("settings/editor/scene_elem.bmp");

	assets_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	assets_treeview->SetListener(AssetListID, this, 0);

	assets_treeview->AddImage("settings/editor/folder.bmp");
	assets_treeview->AddImage("settings/editor/scene_elem.bmp");

	EUIPanel* voPanels = new EUIPanel(lt, 30, 50, 100, 30);

	EUILayout* vo_lt = new EUILayout(voPanels, true);

	EUIPanel* viewportPanels = new EUIPanel(vo_lt, 30, 50, 100, 30);

	outputPanels = new EUITabPanel(vo_lt, 30, 50, 100, 30);
	vo_lt->SetChildSize(outputPanels, 250, false);

	viewport_lt = new EUILayout(viewportPanels, true);

	viewport = new EUIPanel(viewport_lt, 10, 10, 100, 30);
	viewport->SetListener(ViewportID, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	asset_vp_sheet_lt = new EUILayout(viewportPanels, false);

	asset_treeview_panel = new EUIPanel(asset_vp_sheet_lt, 30, 50, 100, 30);
	asset_vp_sheet_lt->SetChildSize(asset_treeview_panel, 200, false);

	EUILayout* panelBLt = new EUILayout(asset_treeview_panel, false);
	asset_treeview = new EUITreeView(panelBLt, 0, 0, 100, 100, false, true);

	asset_treeview->AddImage("settings/editor/folder.bmp");
	asset_treeview->AddImage("settings/editor/scene_elem.bmp");

	asset_treeview->SetListener(AssetListID + 100, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

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

	SceneObject::asset_treeview = asset_treeview;
	SceneObject::cat = objCat;
	SceneObject::objName = objectName;
	SceneObject::popup_menu = popup_menu;

	AddOutputBox("Output");

	//FIXME
	AddOutputBox("Script");

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

	checker_texture = render.LoadTexture("settings/editor/checker.png");
	checker_texture->SetFilters(Texture::Point, Texture::Point);
}

int Editor::Run()
{
	return EUI::Run();
}

void Editor::LogToOutputBox(const char* name, const char* text)
{
	if (output_boxes.find(name) == output_boxes.end())
	{
		Editor::AddOutputBox(name);
	}

	output_boxes["Output"]->AddItem(text, nullptr);
	output_boxes[name]->AddItem(text, nullptr);
}

void Editor::AddOutputBox(const char* name)
{
	EUITabSheet* output_sheet = outputPanels->AddTab(name);
	EUILayout* output_viewport_lt = new EUILayout(output_sheet, true);

	output_boxes[name] = new EUIListBox(output_viewport_lt, 0, 0, 100, 100, false);
}

void Editor::ClearScene()
{
	SelectObject(nullptr, false);
	sceneName.clear();
	scene_treeview->ClearTree();
	assets_treeview->ClearTree();
	ed_scene.Clear();
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

void Editor::SelectObject(SceneObject* obj, bool is_asset)
{
	if (obj == selectedObject)
	{
		return;
	}

	if (in_select_object)
	{
		return;
	}

	in_select_object = true;

	if (selectedObject)
	{
		if (selectedObject->UsingCamera2DPos())
		{
			selectedObject->Camera2DPos() = Sprite::ed_cam_pos;
			Sprite::ed_cam_pos = ed_scene.camera_pos;
		}

		selectedObject->GetMetaData()->HideWidgets();
		selectedObject->EnableTasks(false);
		selectedObject->SetEditMode(false);

		if (isSelectedAsset)
		{
			assets_treeview->SelectItem(nullptr);
		}
		else
		{
			scene_treeview->SelectItem(nullptr);
		}
	}

	selectedObject = obj;
	isSelectedAsset = is_asset;

	bool enabled = (selectedObject != nullptr);
	objectName->Enable(enabled);
	gizmo.enabled = enabled;

	if (selectedObject)
	{
		objectName->SetText(obj->GetName());

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->PrepareWidgets(objCat);
		obj->EnableTasks(true);
		obj->SetEditMode(true);

		if (selectedObject->UsingCamera2DPos())
		{
			Sprite::ed_cam_pos = selectedObject->Camera2DPos();
		}

		if (isSelectedAsset)
		{
			assets_treeview->SelectItem(obj->item);

			asset_treeview->ClearTree();
			int panel_width = ((SceneAsset*)selectedObject)->UseAseetsTree() ? 200 : 1;
			asset_vp_sheet_lt->SetChildSize(asset_treeview_panel, (float)panel_width, false);
			asset_vp_sheet_lt->Resize();
		}
		else
		{
			scene_treeview->SelectItem(obj->item);
		}
	}
	else
	{
		objectName->SetText("");
	}

	ShowVieport();

	in_select_object = false;
}

void Editor::CopyObject(SceneObject* obj, void* parent, bool is_asset)
{
	if (!obj)
	{
		return;
	}

	SceneObject* copy = ed_scene.AddObject(obj->GetClassName(), is_asset);

	copy->Copy(obj);

	SetUniqueName(copy, obj->GetName(), is_asset);

	if (is_asset)
	{
		copy->item = assets_treeview->AddItem(copy->GetName(), 1, copy, parent, -1, false);
		assets_treeview->SelectItem(copy->item);
	}
	else
	{
		copy->item = scene_treeview->AddItem(copy->GetName(), 1, copy, parent, -1, false);
		scene_treeview->SelectItem(copy->item);
	}
}

void Editor::SetUniqueName(SceneObject* obj, const char* name, bool is_asset)
{
	if (!ed_scene.Find(name, is_asset))
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
		unique = (ed_scene.Find(uniqueName, is_asset) == nullptr);
	}

	obj->SetName(uniqueName);
	objectName->SetText(uniqueName);
}

void Editor::CreateSceneObject(const char* name, void* parent, bool is_asset)
{
	SceneObject* obj = ed_scene.AddObject(name, is_asset);
	obj->ApplyProperties();

	obj->Trans().Move(freecamera.pos + Vector(cosf(freecamera.angles.x), sinf(freecamera.angles.y), sinf(freecamera.angles.x)) * 5.0f);

	SetUniqueName(obj, name, is_asset);

	if (is_asset)
	{
		obj->item = assets_treeview->AddItem(obj->GetName(), 1, obj, parent, -1, false);
		assets_treeview->SelectItem(obj->item);
	}
	else
	{
		obj->item = scene_treeview->AddItem(obj->GetName(), 1, obj, parent, -1, false);
		scene_treeview->SelectItem(obj->item);
	}
}

void Editor::DeleteSceneObject(SceneObject* obj)
{
	if (!obj)
	{
		return;
	}

	if (selectedObject == obj)
	{
		SelectObject(nullptr, false);
	}

	scene_treeview->DeleteItem(obj);
	ed_scene.DeleteObject(obj, false);
}

void Editor::ShowVieport()
{
	EUIPanel* vp = isSelectedAsset ? asset_viewport : viewport;
	vp->Show(true);

	SceneObject::vieport = vp;

	if (scene)
	{
		vp = game_viewport;
	}
	else
	{
		viewport_lt->Show(!isSelectedAsset);
		asset_vp_sheet_lt->Show(isSelectedAsset);
	}

	render.GetDevice()->SetVideoMode((int)vp->GetWidth(), (int)vp->GetHeight(), vp->GetNative());
	controls.SetWindow(vp->GetNative());
}

void Editor::DeleteSceneAsset(SceneAsset* obj)
{
	if (!obj)
	{
		return;
	}

	if (selectedObject == obj)
	{
		SelectObject(nullptr, false);
	}

	assets_treeview->DeleteItem(obj->item);
	ed_scene.DeleteObject(obj, true);
}

void Editor::StartScene()
{
	if (selectedObject)
	{
		selectedObject->EnableTasks(false);
		ed_scene.EnableTasks(false);
	}

	Save();

	scripts.Start();

	scene = new Scene();
	scene->Init();
	scene->Load(sceneName.c_str());
	scene->Play();

	Sprite::ed_cam_pos = 0.0f;

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

	scripts.Stop();

	if (selectedObject)
	{
		selectedObject->EnableTasks(true);
		ed_scene.EnableTasks(true);

		if (selectedObject->UsingCamera2DPos())
		{
			Sprite::ed_cam_pos = selectedObject->Camera2DPos();
		}
		else
		{
			Sprite::ed_cam_pos = ed_scene.camera_pos;
		}
	}

	ShowVieport();

	gameWnd = nullptr;
	game_viewport = nullptr;
}

void Editor::Draw(float dt)
{
	render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

	if (!scene && selectedObject && selectedObject->HasOwnTasks() && !selectedObject->Is3DObject())
	{
		Transform2D trans;
		Sprite::FrameState state;
		Sprite::Draw(checker_texture, COLOR_WHITE, Matrix(),
		             0.0f, Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()),
		             0.0f, Vector2((float)render.GetDevice()->GetWidth() / 42.0f, (float)render.GetDevice()->GetHeight() / 42.0f), false);
	}

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

void Editor::LoadNodes(JSONReader* reader, vector<SceneTreeNode>& nodes, const char* group)
{
	while (reader->EnterBlock(group))
	{
		nodes.push_back(SceneTreeNode());
		SceneTreeNode& node = nodes.back();

		reader->Read("index_in_scene", node.index_in_scene);
		reader->Read("name", node.name);
		reader->Read("type", node.type);

		reader->LeaveBlock();
	}
}

void Editor::Load()
{
	ed_scene.Load(sceneName.c_str());

	string scene_tree = sceneName;
	scene_tree.resize(scene_tree.size() - 3);
	scene_tree += "snt";

	JSONReader reader;
	if (reader.Parse(scene_tree.c_str()))
	{
		LoadNodes(&reader, scene_nodes, "scene_nodes");
		LoadNodes(&reader, assets_nodes, "asset_nodes");
	}
	
	RestoreTreeviewNodes();
}

void Editor::SaveNode(JSONWriter* writer, vector<SceneTreeNode>& nodes, const char* group)
{
	writer->StartArray(group);

	for (auto& node : nodes)
	{
		writer->StartBlock(nullptr);

		writer->Write("index_in_scene", node.index_in_scene);
		writer->Write("name", node.name.c_str());
		writer->Write("type", node.type);

		writer->FinishBlock();
	}

	writer->FinishArray();
}

void Editor::Save()
{
	GrabTreeviewNodes();
	ed_scene.Save(sceneName.c_str());
	string scene_tree = sceneName;
	scene_tree.resize(scene_tree.size() - 3);
	scene_tree+="snt";

	JSONWriter writer;
	writer.Start(scene_tree.c_str());

	SaveNode(&writer, scene_nodes, "scene_nodes");
	SaveNode(&writer, assets_nodes, "asset_nodes");
}

void Editor::RestoreTreeviewNodes()
{
	RestoreTreeviewNodes(scene_treeview, scene_nodes, false);
	RestoreTreeviewNodes(assets_treeview, assets_nodes, true);
}

void Editor::RestoreTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& nodes, bool is_asset)
{
	if (nodes.size() == 0)
	{
		for (int i = 0; i < ed_scene.GetObjectsCount(is_asset); i++)
		{
			SceneObject* obj = ed_scene.GetObj(i, is_asset);
			obj->item = treeview->AddItem(obj->GetName(), 1, obj, nullptr, -1, false);
		}
	}
	else
	{
		int index = 0;
		treeview->ClearTree();
		RestoreTreeviewNodes(treeview, nodes, index, nullptr, is_asset);
	}
}

void Editor::RestoreTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& nodes, int& index, void* item, bool is_asset)
{
	SceneTreeNode& node = nodes[index];
	index++;
	
	while (node.type != 2)
	{
		SceneObject* asset = (node.index_in_scene != -1) ? ed_scene.GetObj(node.index_in_scene, is_asset) : nullptr;

		void* child = treeview->AddItem(node.name.c_str(), node.type, asset, item , -1, (node.type == 0));

		if (asset)
		{
			asset->item = child;
		}
		else
		{
			RestoreTreeviewNodes(treeview, nodes, index, child, is_asset);
		}

		node = nodes[index];
		index++;
	}
}

void Editor::GrabTreeviewNodes()
{
	scene_nodes.clear();
	GrabTreeviewNodes(scene_treeview, scene_nodes, nullptr, false);

	assets_nodes.clear();
	GrabTreeviewNodes(assets_treeview, assets_nodes, nullptr, true);
}

void Editor::GrabTreeviewNodes(EUITreeView* treeview, vector<SceneTreeNode>& nodes, void* item, bool is_asset)
{
	int count = treeview->GetItemChildCount(item);

	for (int i = 0; i < count; i++)
	{
		void* child = treeview->GetItemChild(item, i);

		nodes.push_back(SceneTreeNode());
		SceneTreeNode& node = nodes.back();

		treeview->GetItemText(child, node.name);
		SceneAsset* asset = (SceneAsset*)treeview->GetItemPtr(child);

		if (asset)
		{
			node.type = 1;
			node.index_in_scene = ed_scene.GetObjectIndex(asset, is_asset);
		}
		else
		{
			GrabTreeviewNodes(treeview, nodes, child, is_asset);
		}
	}

	nodes.push_back(SceneTreeNode());
	SceneTreeNode& node = nodes.back();
	node.type = 2;
}

void Editor::CreatePopup(EUITreeView* treeview, int x, int y, bool is_asset)
{
	popup_menu->StartMenu(true);

	popup_menu->AddItem(3500, "Create Folder");

	if (!popup_scene_item && popup_item)
	{
		popup_menu->AddItem(3501, "Create Folder as child");
	}

	popup_menu->StartSubMenu("Create");

	int id = 1000;

	if (is_asset)
	{
		for (const auto& decl : ClassFactorySceneAsset::Decls())
		{
			popup_menu->AddItem(id, decl->GetShortName());
			id++;
		}
	}
	else
	{
		for (const auto& decl : ClassFactorySceneObject::Decls())
		{
			popup_menu->AddItem(id, decl->GetShortName());
			id++;
		}
	}

	popup_menu->EndSubMenu();

	if (!popup_scene_item && popup_item)
	{
		popup_menu->StartSubMenu("Create as child");

		if (is_asset)
		{
			int id = 500;
			for (const auto& decl : ClassFactorySceneAsset::Decls())
			{
				popup_menu->AddItem(id, decl->GetShortName());
				id++;
			}
		}
		else
		{
			int id = 500;
			for (const auto& decl : ClassFactorySceneObject::Decls())
			{
				popup_menu->AddItem(id, decl->GetShortName());
				id++;
			}
		}

		popup_menu->EndSubMenu();
	}

	if (!popup_scene_item && object_to_copy)
	{
		popup_menu->AddItem(3505, "Paste");
		popup_menu->AddItem(3504, "Paste as child");
	}
	else
	if (popup_scene_item)
	{
		popup_menu->AddSeparator();
		popup_menu->AddItem(3502, "Duplicate");
		popup_menu->AddItem(3503, "Copy");

		if (object_to_copy)
		{
			popup_menu->AddItem(3505, "Paste");
		}
	}

	if (popup_item)
	{
		popup_menu->AddItem(3506, "Delete");
	}

	popup_menu->ShowAsPopup(treeview, x, y);
}

void Editor::ProcesTreeviewPopup(EUITreeView* treeview, int id, bool is_asset)
{
	if (id == 3500)
	{
		treeview->AddItem("Folder", 0, nullptr, popup_item ? treeview->GetItemParent(popup_item) : nullptr, -1, true);
	}

	if (id == 3501)
	{
		treeview->AddItem("Folder", 0, nullptr, popup_item, -1, true);
	}

	if (id == 3502 && popup_scene_item)
	{
		CopyObject(popup_scene_item, treeview->GetItemParent(popup_scene_item->item), is_asset);
	}

	if (id == 3503 && popup_scene_item)
	{
		object_to_copy = popup_scene_item;
	}

	if (id == 3504 && object_to_copy)
	{
		CopyObject(object_to_copy, popup_item, is_asset);
	}

	if (id == 3505 && object_to_copy)
	{
		CopyObject(object_to_copy, treeview->GetItemParent(popup_item), is_asset);
	}

	if (id == 3506)
	{
		allow_delete_objects_by_tree = true;
		treeview->DeleteItem(popup_item);
		allow_delete_objects_by_tree = false;
	}

	if (id >= 500 && id <= 900)
	{
		CreateSceneObject(is_asset ? ClassFactorySceneAsset::Decls()[id - 500]->GetName() : ClassFactorySceneObject::Decls()[id - 500]->GetName(), popup_item, is_asset);
	}

	if (id >= 1000 && id <= 1400)
	{
		CreateSceneObject(is_asset ? ClassFactorySceneAsset::Decls()[id - 1000]->GetName() : ClassFactorySceneObject::Decls()[id - 1000]->GetName(), treeview->GetItemParent(popup_item), is_asset);
	}
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

		if (selectedObject && !scene && selectedObject->Is3DObject())
		{
			if (allowCopy && controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active))
			{
				if (!selectedObject->Trans().IsEqual(gizmo.transform))
				{
					allowCopy = false;
					CopyObject(selectedObject, selectedObject->item, false);
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
	}
}

void Editor::OnMenuItem(EUIMenu* sender, int activated_id)
{
	if (activated_id == MenuNewID)
	{
		ClearScene();
	}

	if (activated_id == MenuSaveID && sceneName.size() != 0)
	{
		Save();
	}

	if ((activated_id == MenuSaveID && sceneName.size() == 0) ||
		activated_id == MenuSaveAsID)
	{
		const char* fileName = EUI::EUI::OpenSaveDialog(mainWnd->GetNative(), "Scene file", "scn");

		if (fileName)
		{
			sceneName = fileName;
		}

		Save();
	}

	if (activated_id == MenuOpenID)
	{
		const char* fileName = EUI::OpenOpenDialog(mainWnd->GetNative(), "Scene file", "scn");

		if (fileName)
		{
			ClearScene();
			sceneName = fileName;
			Load();
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

	if (popup_parent == asset_treeview && selectedObject && isSelectedAsset)
	{
		((SceneAsset*)selectedObject)->OnAssetTreePopupItem(activated_id);
	}
	else
	if (popup_parent == scene_treeview || popup_parent == assets_treeview)
	{
		ProcesTreeviewPopup((EUITreeView*)popup_parent, activated_id, (popup_parent == assets_treeview));
	}
	else
	if (selectedObject)
	{
		selectedObject->OnPopupMenuItem(activated_id);
	}
}

void Editor::OnUpdate(EUIWidget* sender)
{
	core.CountDeltaTime();
	float dt = core.GetDeltaTime();

	if (!scene)
	{
		asset_drag_as_inst = controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active, true);

		freecamera.mode_2d = selectedObject ? !selectedObject->Is3DObject() : false;

		if (!freecamera.mode_2d && (!selectedObject || !selectedObject->HasOwnTasks()))
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

		if (selectedObject)
		{
			selectedObject->CheckProperties();
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
		if (selectedObject && selectedObject->HasOwnTasks())
		{
			ed_scene.EnableTasks(false);
			selectedObject->Tasks(true)->Execute(dt);
		}
		else
		{
			ed_scene.EnableTasks(true);
			ed_scene.Execute(dt);
		}
	}

	render.DebugPrintText(5.0f, COLOR_WHITE, "%i",core.GetFPS());

	core.Update();
}

void Editor::OnEditBoxStopEditing(EUIEditBox* sender)
{
	if (sender == objectName)
	{
		if (selectedObject)
		{
			if (StringUtils::IsEqual(objectName->GetText(), selectedObject->GetName()))
			{
				return;
			}

			SetUniqueName(selectedObject, objectName->GetText(), false);
			scene_treeview->SetItemText(selectedObject->item, objectName->GetText());
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

bool Editor::OnTreeViewItemDragged(EUITreeView* sender, EUITreeView* target, void* item, int prev_child_index, void* parent, int child_index)
{
	if ((sender == assets_treeview || sender == asset_treeview ) && target == asset_treeview && selectedObject && isSelectedAsset)
	{
		return ((SceneAsset*)selectedObject)->OnAssetTreeViewItemDragged((sender == assets_treeview), (SceneAsset*)sender->GetItemPtr(item), prev_child_index, (SceneObject*)target->GetItemPtr(parent), child_index);
	}

	if (sender == target)
	{
		return true;
	}

	if (sender == assets_treeview && target == scene_treeview && selectedObject && isSelectedAsset)
	{
		SceneObject* inst = ((SceneAsset*)selectedObject)->CreateInstance();
		inst->item = scene_treeview->AddItem(inst->GetName(), 1, inst, nullptr, -1, false);
		scene_treeview->SelectItem(inst->item);
	}

	return false;
}

void Editor::OnTreeViewSelChange(EUITreeView* sender, void* item)
{
	if (sender == asset_treeview && selectedObject && isSelectedAsset)
	{
		((SceneAsset*)selectedObject)->OnAssetTreeSelChange((SceneAsset*)asset_treeview->GetItemPtr(item));
	}
	else
	if (sender == scene_treeview)
	{
		SelectObject((SceneObject*)sender->GetItemPtr(item), false);
	}
	else
	if (sender == assets_treeview)
	{
		SceneAsset* asset = (SceneAsset*)sender->GetItemPtr(item);

		if (asset_drag_as_inst)
		{
			if (selectedObject != asset)
			{
				selectedAsset2Drag = asset;
			}
		}
		else
		{
			SelectObject(asset, true);
		}
	}
}

void Editor::OnTreeReCreateItem(EUITreeView* sender, void* item, void* ptr)
{
	if (sender == scene_treeview || sender == assets_treeview)
	{
		SceneObject* obj = (SceneObject*)ptr;

		if (obj)
		{
			obj->item = item;
		}
	}
}

void Editor::OnTreeDeleteItem(EUITreeView* sender, void* item, void* ptr)
{
	if ((sender == scene_treeview || sender == assets_treeview) && allow_delete_objects_by_tree)
	{
		if (ptr)
		{
			SceneObject* object = (SceneObject*)ptr;

			if (object == selectedObject)
			{
				SelectObject(nullptr, false);
			}

			ed_scene.DeleteObject(object, (sender == assets_treeview));
		}
	}
}

void Editor::OnTreeViewRightClick(EUITreeView* sender, int x, int y, void* item, int child_index)
{
	if (sender == asset_treeview && selectedObject && isSelectedAsset)
	{
		popup_parent = sender;
		((SceneAsset*)selectedObject)->OnAssetTreeRightClick(x, y, (SceneAsset*)sender->GetItemPtr(item), child_index);
	}

	if (sender == scene_treeview || sender == assets_treeview)
	{
		popup_item = item;
		popup_scene_item = (SceneAsset*)sender->GetItemPtr(item);
		popup_child_index = child_index;

		popup_parent = sender;
		CreatePopup(sender, x, y, (sender == assets_treeview));
	}
}

void Editor::OnTreeViewSelItemTextChanged(EUITreeView* sender, void* item, const char* text)
{
	if (sender == scene_treeview || sender == assets_treeview)
	{
		SceneObject* object = (SceneObject*)sender->GetItemPtr(item);

		if (object)
		{
			object->SetName(text);
		}

		sender->SetItemText(item, text);
	}
}