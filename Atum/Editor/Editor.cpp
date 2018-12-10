
#include "Editor.h"

char appdir[1024];

EUITabPanel* Editor::outputPanels = nullptr;
map<string, EUIListBox*> Editor::output_boxes;

extern Scene* hack_scene;

void Editor::Init()
{
	GetCurrentDirectory(1024, appdir);

	if (StringUtils::IsEqual(EUI::GetName(), "EUI_DX11"))
	{
		EUI::Init("settings/EUI/", "theme_dx11.dat");
	}
	else
	{
		EUI::Init("settings/EUI/", "theme.dat");
	}

	ClassFactorySceneObject::Sort();
	ClassFactorySceneAsset::Sort();

	mainWnd = new EUIWindow("Atum", "settings//editor//icon.ico", EUIWindow::Normal, true, 30, 30, 800, 600);
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

	EUILayout* lt_main = new EUILayout(mainWnd, true);

	EUIPanel* tool_panel = new EUIPanel(lt_main, 10, 10, 100, 30);
	lt_main->SetChildSize(tool_panel, 30, false);

	playBtn = new EUIButton(tool_panel, "Play", 5, 5, 30, 20);
	playBtn->SetListener(-1, this, 0);

	moveModeBtn = new EUIButton(tool_panel, "2D", 50, 5, 30, 20);
	moveModeBtn->SetPushable(true);
	moveModeBtn->SetListener(-1, this, 0);

	trans3d_gizmo = new EUIPanel(tool_panel, 80, 0, 150, 30);

	moveBtn = new EUIButton(trans3d_gizmo, "Move", 5, 5, 30, 20);
	moveBtn->SetPushable(true);
	moveBtn->SetListener(-1, this, 0);

	rotateBtn = new EUIButton(trans3d_gizmo, "Rot", 40, 5, 30, 20);
	rotateBtn->SetPushable(true);
	rotateBtn->SetListener(-1, this, 0);

	globalBtn = new EUIButton(trans3d_gizmo, "Glob", 75, 5, 30, 20);
	globalBtn->SetPushable(true);
	globalBtn->SetListener(-1, this, 0);

	localBtn = new EUIButton(trans3d_gizmo, "Loc", 110, 5, 30, 20);
	localBtn->SetPushable(true);
	localBtn->SetListener(-1, this, 0);

	trans2d_gizmo = new EUIPanel(tool_panel, 80, 0, 150, 30);

	EUILabel* label = new EUILabel(trans2d_gizmo, "AlignX", 5, 7, 40, 20);
	x_align = new EUIEditBox(trans2d_gizmo, "0", 45, 5, 30, 20, EUIEditBox::InputUInteger);
	x_align->SetListener(-1, this, 0);
	label = new EUILabel(trans2d_gizmo, "AlignY", 82, 7, 40, 20);
	y_align = new EUIEditBox(trans2d_gizmo, "0", 120, 5, 30, 20, EUIEditBox::InputUInteger);
	y_align->SetListener(-1, this, 0);


	EUIPanel* main_panel = new EUIPanel(lt_main, 10, 10, 100, 30);

	EUILayout* lt = new EUILayout(main_panel, false);

	EUIPanel* toolsPanel2 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel2, 250, false);

	EUILayout* leftPanelLt = new EUILayout(toolsPanel2, true);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(-1, this, 0);

	EUITabSheet* sheet = tabPanel->AddTab("Scene");

	EUILayout* scene_lt = new EUILayout(sheet, true);

	scene_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	scene_lt->SetChildSize(scene_treeview, 0.5, true);
	scene_treeview->SetListener(-1, this, 0);

	scene_treeview->AddImage("settings/editor/folder.png");
	scene_treeview->AddImage("settings/editor/scene_elem.png");

	assets_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	assets_treeview->SetListener(-1, this, 0);

	assets_treeview->AddImage("settings/editor/folder.png");
	assets_treeview->AddImage("settings/editor/scene_elem.png");

	EUIPanel* voPanels = new EUIPanel(lt, 30, 50, 100, 30);

	EUILayout* vo_lt = new EUILayout(voPanels, true);

	EUIPanel* viewportPanels = new EUIPanel(vo_lt, 30, 50, 100, 30);

	outputPanels = new EUITabPanel(vo_lt, 30, 50, 100, 30);
	vo_lt->SetChildSize(outputPanels, 250, false);

	vp_sheet_lt = new EUILayout(viewportPanels, false);

	asset_treeview_panel = new EUIPanel(vp_sheet_lt, 30, 50, 100, 30);
	vp_sheet_lt->SetChildSize(asset_treeview_panel, 200, false);

	EUILayout* panelBLt = new EUILayout(asset_treeview_panel, false);
	asset_treeview = new EUITreeView(panelBLt, 0, 0, 100, 100, false, true);

	asset_treeview->AddImage("settings/editor/folder.png");
	asset_treeview->AddImage("settings/editor/scene_elem.png");

	asset_treeview->SetListener(-1, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	viewport = new EUIPanel(vp_sheet_lt, 10, 10, 100, 30);
	viewport->SetListener(-1, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	EUIPanel* toolsPanel3 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel3, 230, false);

	EUILayout* object_lt = new EUILayout(toolsPanel3, true);

	objCat = new EUICategories(object_lt, 0, 0, 100, 100);

	objCmpWgt.Init(objCat);

	SceneObject::ed_asset_treeview = asset_treeview;
	SceneObject::ed_obj_cat = objCat;
	SceneObject::ed_popup_menu = popup_menu;

	AddOutputBox("Output");

	//FIXME
	AddOutputBox("Script");

	core.Init(EUI::GetRenderDevice());

	render.AddExecutedLevelPool(1);

	renderTaskPool = render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&Editor::Draw);

	freecamera.Init();
	ed_scene.Init();

	Sprite::Init();

	gizmo.Init();
	editor_drawer.Init();

	UpdateGizmoToolbar();

	mainWnd->Show(true);
	mainWnd->Maximaze();

	vp_sheet_lt->SetChildSize(asset_treeview_panel, (float)asset_panel_width, false);
	vp_sheet_lt->Resize();
	ShowVieport();

	checker_texture = render.LoadTexture("settings/editor/checker.png");
	checker_texture->SetFilters(Texture::Point, Texture::Point);

	ShowGizmoControls(0);
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

void Editor::ShowGizmoControls(int mode)
{
	trans3d_gizmo->Show(mode == 1);
	trans2d_gizmo->Show(mode == 2);
}

void Editor::ClearScene()
{
	SelectObject(nullptr, false);
	sceneName.clear();
	scene_nodes.clear();
	assets_nodes.clear();
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
			Sprite::ed_cam_pos = ed_scene.camera2d_pos;
		}

		selectedObject->ShowPropWidgets(nullptr);
		selectedObject->EnableTasks(false);
		selectedObject->SetEditMode(false);

		for (auto comp : selectedObject->components)
		{
			comp->SetEditMode(false);
		}

		if (isSelectedAsset)
		{
			//assets_treeview->SelectItem(nullptr);
		}
		else
		{
			//scene_treeview->SelectItem(nullptr);
		}
	}

	selectedObject = obj;
	isSelectedAsset = is_asset;

	if (selectedObject)
	{
		obj->ShowPropWidgets(objCat);
		obj->EnableTasks(true);
		obj->SetEditMode(true);

		if (selectedObject->UsingCamera2DPos())
		{
			ed_scene.camera2d_pos = Sprite::ed_cam_pos;
			Sprite::ed_cam_pos = selectedObject->Camera2DPos();
		}

		int panel_width = 1;

		if (isSelectedAsset)
		{
			//assets_treeview->SelectItem(obj->item);

			asset_treeview->ClearTree();
			panel_width = ((SceneAsset*)selectedObject)->UseAseetsTree() ? 200 : 1;

			if (panel_width > 1)
			{
				((SceneAsset*)selectedObject)->PreapreAssetTree();
			}
		}
		else
		{
			//scene_treeview->SelectItem(obj->item);
		}

		if (asset_panel_width != panel_width)
		{
			asset_panel_width = panel_width;
			vp_sheet_lt->SetChildSize(asset_treeview_panel, (float)panel_width, false);
			vp_sheet_lt->Resize();
			ShowVieport();
		}
	}

	objCmpWgt.Prepare(selectedObject);

	in_select_object = false;
}

void Editor::CopyObject(SceneObject* obj, void* parent, bool is_asset)
{
	if (!obj)
	{
		return;
	}

	SceneObject* copy = ed_scene.AddObject(obj->class_name, is_asset);
	ed_scene.GenerateUID(copy, is_asset);
	copy->Copy(obj);

	string name = obj->GetName();
	name += "_copy";

	copy->SetName(name.c_str());

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

void Editor::CreateSceneObject(const char* name, void* parent, bool is_asset)
{
	SceneObject* obj = ed_scene.AddObject(name, is_asset);
	ed_scene.GenerateUID(obj, is_asset);
	obj->ApplyProperties();

	if (obj->Is3DObject())
	{
		obj->Trans().Move(freecamera.pos + Vector(cosf(freecamera.angles.x), sinf(freecamera.angles.y), sinf(freecamera.angles.x)) * 5.0f);
	}

	obj->SetName(is_asset ? ClassFactorySceneAsset::Find(name)->GetShortName() : ClassFactorySceneObject::Find(name)->GetShortName());

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

void Editor::ShowVieport()
{
	EUIPanel* vp = viewport;

	SceneObject::ed_vieport = vp;

	if (scene)
	{
		vp = game_viewport;
	}

	render.GetDevice()->SetVideoMode((int)vp->GetWidth(), (int)vp->GetHeight(), vp->GetNative());
	vp->SetTexture(render.GetDevice()->GetBackBuffer());
}

void Editor::StartScene()
{
	if (selectedObject)
	{
		selectedObject->EnableTasks(false);
	}

	ed_scene.EnableTasks(false);

	Save();

	scripts.Start();

	scene = new Scene();
	scene->Init();
	scene->Load(sceneName.c_str());
	if (scene->Play())
	{
		mainWnd->Enable(false);
		hack_scene = scene;

		Sprite::ed_cam_pos = 0.0f;

		gameWnd = new EUIWindow("Game", "", EUIWindow::PopupWithCloseBtn, true, 0, 0, 1024, 768);
		gameWnd->SetListener(-1, this, 0);

		EUILayout* lt = new EUILayout(gameWnd, false);

		game_viewport = new EUIPanel(lt, 0, 0, 800, 600);
		game_viewport->SetListener(-1, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

		game_viewport->SetFocused();

		gameWnd->Show(true);
		gameWnd->SetAtScreenCenter();
	}
	else
	{
		StopScene();
	}
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
	}

	ed_scene.EnableTasks(true);
	
	if (selectedObject && selectedObject->UsingCamera2DPos())
	{
		Sprite::ed_cam_pos = selectedObject->Camera2DPos();
	}
	else
	{
		Sprite::ed_cam_pos = ed_scene.camera2d_pos;
	}

	ShowVieport();

	mainWnd->Enable(true);

	gameWnd = nullptr;
	game_viewport = nullptr;
}

void Editor::Draw(float dt)
{
	if (!scene)
	{
		render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		if (selectedObject && selectedObject->HasOwnTasks() && !selectedObject->Is3DObject())
		{
			Transform2D trans;
			Sprite::FrameState state;
			Sprite::Draw(checker_texture, COLOR_WHITE, Matrix(),
			             0.0f, Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()),
			             Vector2(((int)(Sprite::ed_cam_pos.x) % 42) / 42.0f, ((int)(1.0f - Sprite::ed_cam_pos.y) % 42) / 42.0f),
			             Vector2((float)render.GetDevice()->GetWidth() / 42.0f, (float)render.GetDevice()->GetHeight() / 42.0f), false);
		}
	}
	else
	{
		render.GetDevice()->Clear(false , COLOR_GRAY, true, 1.0f);
	}

	render.ExecutePool(ExecuteLevels::Camera, dt);
	render.ExecutePool(ExecuteLevels::Prepare, dt);
	render.ExecutePool(ExecuteLevels::Geometry, dt);
	render.ExecutePool(ExecuteLevels::DebugGeometry, dt);
	render.ExecutePool(ExecuteLevels::Sprites, dt);
	render.ExecutePool(ExecuteLevels::Sprites + 1, dt);
	render.ExecutePool(ExecuteLevels::PostProcess, dt);
	render.ExecutePool(ExecuteLevels::GUI, dt);
	render.ExecutePool(ExecuteLevels::Debug, dt);

	render.GetDevice()->Present();
}

void Editor::LoadNodes(JSONReader* reader, vector<SceneTreeNode>& nodes, const char* group)
{
	while (reader->EnterBlock(group))
	{
		nodes.push_back(SceneTreeNode());
		SceneTreeNode& node = nodes.back();

		reader->Read("uid", node.uid);
		reader->Read("name", node.name);
		reader->Read("type", node.type);

		reader->LeaveBlock();
	}
}

void Editor::Load()
{
	ed_scene.Load(sceneName.c_str());
	Sprite::ed_cam_pos = ed_scene.camera2d_pos;
	freecamera.angles = ed_scene.camera3d_angles;
	freecamera.pos = ed_scene.camera3d_pos;
	moveModeBtn->SetPushed(ed_scene.move_mode == 1);
	x_align->SetText(ed_scene.gizmo2d_align_x);
	y_align->SetText(ed_scene.gizmo2d_align_y);
	gizmo.align2d = Vector2((float)ed_scene.gizmo2d_align_x, (float)ed_scene.gizmo2d_align_x);

	string scene_tree = sceneName;
	scene_tree.resize(scene_tree.size() - 3);
	scene_tree += "snt";

	JSONReader reader;
	if (reader.Parse(scene_tree.c_str()))
	{
		LoadNodes(&reader, scene_nodes, "scene_nodes");
		LoadNodes(&reader, assets_nodes, "asset_nodes");
	}
	else
	{
		for (int i = 0; i < ed_scene.GetObjectsCount(false); i++)
		{
			SceneTreeNode node;

			node.type = 1;
			node.uid = ed_scene.GetObj(i, false)->GetUID();
			node.name = ed_scene.GetObj(i, false)->GetName();

			scene_nodes.push_back(node);
		}

		SceneTreeNode node;

		node.type = 2;
		node.uid = -1;

		scene_nodes.push_back(node);

		for (int i = 0; i < ed_scene.GetObjectsCount(true); i++)
		{
			SceneTreeNode node;

			node.type = 1;
			node.uid = ed_scene.GetObj(i, false)->GetUID();
			node.name = ed_scene.GetObj(i, true)->GetName();

			assets_nodes.push_back(node);
		}

		assets_nodes.push_back(node);
	}
	
	RestoreTreeviewNodes();
}

void Editor::SaveNode(JSONWriter* writer, vector<SceneTreeNode>& nodes, const char* group)
{
	writer->StartArray(group);

	for (auto& node : nodes)
	{
		writer->StartBlock(nullptr);

		writer->Write("uid", node.uid);
		writer->Write("name", node.name.c_str());
		writer->Write("type", node.type);

		writer->FinishBlock();
	}

	writer->FinishArray();
}

void Editor::Save()
{
	if (selectedObject && selectedObject->UsingCamera2DPos())
	{
		selectedObject->Camera2DPos() = Sprite::ed_cam_pos;
	}
	else
	{
		ed_scene.camera2d_pos = Sprite::ed_cam_pos;
	}

	ed_scene.camera3d_angles = freecamera.angles;
	ed_scene.camera3d_pos = freecamera.pos;

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
			obj->AddChildsToTree(treeview);
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
		SceneObject* obj = (node.uid != -1) ? ed_scene.FindByUID(node.uid, 0, is_asset) : nullptr;

		void* child = treeview->AddItem(node.name.c_str(), node.type, obj, item , -1, (node.type == 0));

		if (obj)
		{
			obj->item = child;
			obj->AddChildsToTree(treeview);
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
		SceneObject* obj = (SceneObject*)treeview->GetItemPtr(child);

		if (obj)
		{
			node.type = 1;
			node.uid = obj->GetUID();
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

	popup_menu->StartSubMenu("Create");

	if (is_asset)
	{
		int id = MenuSceneAssetID;
		for (const auto& decl : ClassFactorySceneAsset::Decls())
		{
			popup_menu->AddItem(id, decl->GetShortName());
			id++;
		}
	}
	else
	{
		int id = MenuSceneObjectID;
		for (const auto& decl : ClassFactorySceneObject::Decls())
		{
			if (!StringUtils::StrStr(decl->GetName(), "Inst"))
			{
				popup_menu->AddItem(id, decl->GetShortName());
			}

			id++;
		}
	}

	popup_menu->EndSubMenu();

	if (!popup_scene_item && object_to_copy)
	{
		popup_menu->AddItem(3505, "Paste");
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
	void* parent_item = popup_item;

	if (popup_scene_item)
	{
		parent_item = popup_item ? treeview->GetItemParent(popup_item) : nullptr;
	}

	if (id == 3500)
	{
		treeview->AddItem("Folder", 0, nullptr, parent_item, -1, true);
	}

	if (id == 3502 && popup_scene_item)
	{
		CopyObject(popup_scene_item, popup_scene_item ? treeview->GetItemParent(popup_scene_item->item) : nullptr, is_asset);
	}

	if (id == 3503 && popup_scene_item)
	{
		object_to_copy = popup_scene_item;
	}

	if (id == 3505 && object_to_copy)
	{
		CopyObject(object_to_copy, parent_item, is_asset);
	}

	if (id == 3506)
	{
		allow_delete_objects_by_tree = true;
		treeview->DeleteItem(popup_item);
		allow_delete_objects_by_tree = false;
	}

	if (MenuSceneObjectID <= id && id <= MenuLastSceneID)
	{
		CreateSceneObject(is_asset ? ClassFactorySceneAsset::Decls()[id - MenuSceneAssetID]->GetName() : ClassFactorySceneObject::Decls()[id - MenuSceneObjectID]->GetName(), parent_item, is_asset);
	}

	popup_item = nullptr;
	popup_scene_item = nullptr;
}

void Editor::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender == viewport || sender == game_viewport)
	{
		controls.OverrideMousePos(mx, my);

		Vector2 ms((float)mx, (float)my);

		if (controls.DebugKeyPressed("MS_BTN1", Controls::Active) && controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
		{
			Sprite::ed_cam_pos -= ms - prev_ms;
		}

		if (!rect_select)
		{
			gizmo.OnMouseMove(Vector2((float)mx, (float)my));

			if (selectedObject)
			{
				selectedObject->OnMouseMove(ms - prev_ms);
			}

			if (selectedObject && selectedObject->Is3DObject() && !isSelectedAsset && !scene)
			{
				if (allowCopy && controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active))
				{
					allowCopy = false;
					//CopyObject(selectedObject, scene_treeview->GetItemParent(selectedObject->item), false);
				}

				selectedObject->Trans() = gizmo.GetTrans3D();
			}
		}
	}

	prev_ms = Vector2((float)mx, (float)my);
}

void Editor::OnLeftMouseDown(EUIWidget* sender, int mx, int my)
{
	prev_ms = Vector2((float)mx, (float)my);

	if (sender == viewport || sender == game_viewport)
	{
		if (controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active) && (!isSelectedAsset || !selectedObject))
		{
			if (selectedObject && !selectedObject->IsEditMode())
			{
				for (auto comp : selectedObject->components)
				{
					if (comp->IsEditMode())
					{
						comp->CheckSelection({ (float)mx, (float)my });
						break;
					}
				}
			}
			else
			if (!selectedObject || !selectedObject->HasOwnTasks())
			{
				SelectObject(ed_scene.CheckSelection({ (float)mx, (float)my }), false);
			}
		}
		else
		{
			sender->CaptureMouse();

			if (controls.DebugKeyPressed("KEY_LALT", Controls::Active))
			{
				rect_select = true;
				ms_rect = prev_ms;
			}
			else
			{
				allowCopy = true;
				gizmo.OnLeftMouseDown();

				if (selectedObject)
				{
					selectedObject->OnLeftMouseDown(prev_ms);
				}
			}
		}

		sender->SetFocused();
	}
}

void Editor::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == viewport || sender == game_viewport)
	{
		sender->ReleaseMouse();

		if (rect_select)
		{
			rect_select = false;

			if (selectedObject)
			{
				selectedObject->OnRectSelect(Vector2(fmin(prev_ms.x, ms_rect.x), fmin(prev_ms.y, ms_rect.y)),
				                             Vector2(fmax(prev_ms.x, ms_rect.x), fmax(prev_ms.y, ms_rect.y)));
			}
		}
		else
		{
			gizmo.OnLeftMouseUp();

			if (selectedObject)
			{
				selectedObject->OnLeftMouseUp();
			}
		}
	}

	if (sender == moveBtn)
	{
		gizmoMove = true;
		UpdateGizmoToolbar();
	}

	if (sender == rotateBtn)
	{
		gizmoMove = false;
		UpdateGizmoToolbar();
	}

	if (sender == globalBtn)
	{
		gizmoGlobal = true;
		UpdateGizmoToolbar();
	}

	if (sender == localBtn)
	{
		gizmoGlobal = false;
		UpdateGizmoToolbar();
	}

	if (sender == moveModeBtn)
	{
		ed_scene.move_mode = moveModeBtn->IsPushed() ? 1 : 0;
	}

	if (sender == playBtn)
	{
		StartScene();
	}
}

void Editor::OnRightMouseDown(EUIWidget* sender, int mx, int my)
{
	popup_parent = nullptr;

	prev_ms = Vector2((float)mx, (float)my);

	if (sender == viewport)
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
	if (sender == viewport)
	{
		sender->ReleaseMouse();

		if (selectedObject)
		{
			selectedObject->OnRightMouseUp();
		}
	}
}

void Editor::OnEditBoxStopEditing(EUIEditBox* sender)
{
	if (sender == x_align)
	{
		ed_scene.gizmo2d_align_x = x_align->GetAsInt();
		gizmo.align2d.x = (float)ed_scene.gizmo2d_align_x;
	}

	if (sender == y_align)
	{
		ed_scene.gizmo2d_align_y = y_align->GetAsInt();
		gizmo.align2d.y = (float)ed_scene.gizmo2d_align_y;
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
			Save();
		}
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
	if (scene && sender != game_viewport)
	{
		return;
	}

	ShowVieport();

	core.CountDeltaTime();
	float dt = core.GetDeltaTime();

	if (!scene)
	{
		asset_drag_as_inst = controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active, true);

		freecamera.mode_2d = selectedObject ? !selectedObject->Is3DObject() : (ed_scene.move_mode == 1);

		if (!freecamera.mode_2d && (!selectedObject || !selectedObject->HasOwnTasks()))
		{
			for (int i = 0; i <= 20; i++)
			{
				float pos = (float)i - 10.0f;

				render.DebugLine(Vector(pos, 0.0f, -10.0f), COLOR_WHITE, Vector(pos, 0.0f, 10.0f), COLOR_WHITE);
				render.DebugLine(Vector(-10.0f, 0.0f, pos), COLOR_WHITE, Vector(10.0f, 0.0f, pos), COLOR_WHITE);
			}
		}

		if (rect_select)
		{
			render.DebugRect2D(prev_ms, ms_rect, COLOR_YELLOW);
		}

		freecamera.Update(dt);

		if (gizmo.IsEnabled() == !(trans3d_gizmo->IsVisible() || trans2d_gizmo->IsVisible()))
		{
			ShowGizmoControls(gizmo.IsEnabled() ? (gizmo.IsTrans2D() ? 2 : 1) : 0);
		}

		gizmo.Render();

		if (selectedObject)
		{
			selectedObject->CheckProperties();
		}
	}

	if (viewport->IsFocused())
	{
		Color color(1.0, 0.65f, 0.0f, 1.0f);

		for (float i = 0; i < 3.0f; i+=1.0f)
		{
			render.DebugLine2D(Vector2(0.5f, 0.5f + i), color, Vector2((float)render.GetDevice()->GetWidth(), 0.5f + i), color);
			render.DebugLine2D(Vector2(0.5f, (float)render.GetDevice()->GetHeight() - 0.5f - i), color, Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight() - 0.5f - i), color);
			render.DebugLine2D(Vector2(0.5f + i, 0.5f), color, Vector2(0.5f + i, (float)render.GetDevice()->GetHeight()), color);
			render.DebugLine2D(Vector2((float)render.GetDevice()->GetWidth() - i - 0.5f, 0.5f), color, Vector2((float)render.GetDevice()->GetWidth() - i - 0.5f, (float)render.GetDevice()->GetHeight()), color);
		}
	}

	controls.SetFocused(scene ? game_viewport->IsFocused() : viewport->IsFocused());

	render.DebugPrintText(5.0f, COLOR_WHITE, "%i", core.GetFPS());

	core.Update();

	if (scene)
	{
		scene->Execute(dt);
	}
	else
	{
		if (selectedObject)
		{
			void* sel_item = isSelectedAsset ? assets_treeview->GetSelectedItem() : scene_treeview->GetSelectedItem();
			void* item_ptr = isSelectedAsset ? assets_treeview->GetItemPtr(sel_item) : scene_treeview->GetItemPtr(sel_item);

			if (selectedObject == item_ptr)
			{
				string name;

				if (isSelectedAsset)
				{
					assets_treeview->GetItemText(sel_item, name);
				}
				else
				{
					scene_treeview->GetItemText(sel_item, name);
				}

				if (!StringUtils::IsEqual(selectedObject->GetName(), name.c_str()))
				{
					if (isSelectedAsset)
					{
						assets_treeview->SetItemText(sel_item, selectedObject->GetName());
					}
					else
					{
						scene_treeview->SetItemText(sel_item, selectedObject->GetName());
					}
				}
			}
		}

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
}

void Editor::OnResize(EUIWidget* sender)
{
	if (sender == viewport || sender == game_viewport)
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

bool Editor::OnTreeViewItemDragged(EUITreeView* sender, EUIWidget* target, void* item, int prev_child_index, void* parent, int child_index)
{
	if ((sender == assets_treeview || sender == scene_treeview) && target == viewport)
	{
		if (selectedObject)
		{
			int x, y;
			viewport->GetMousePos(x, y);
			Vector2 ms((float)x, (float)y);

			selectedObject->OnDragObjectFromTreeView(sender == scene_treeview, (SceneObject*)sender->GetItemPtr(item), ms);
		}

		return false;
	}

	if ((sender == assets_treeview || sender == asset_treeview ) && target == asset_treeview && selectedObject && isSelectedAsset)
	{
		return ((SceneAsset*)selectedObject)->OnAssetTreeViewItemDragged((sender == assets_treeview), (SceneAsset*)sender->GetItemPtr(item), prev_child_index, (SceneObject*)asset_treeview->GetItemPtr(parent), child_index);
	}

	if (sender == target)
	{
		SceneObject* obj = (SceneObject*)sender->GetItemPtr(item);
		return obj ? !obj->AddedToTreeByParent() : true;
	}

	if (sender == assets_treeview && target == scene_treeview && assets_treeview->GetItemPtr(item))
	{
		SceneAsset* asset = (SceneAsset*)assets_treeview->GetItemPtr(item);

		if (controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active, true))
		{
			SceneObject* obj = (SceneObject*)scene_treeview->GetItemPtr(scene_treeview->GetItemChild(parent, child_index - 1));

			if (obj && StringUtils::IsEqual(asset->inst_class_name, obj->class_name))
			{
				SceneObjectInst* inst = (SceneObjectInst*)obj;
				inst->asset_uid = asset->GetUID();

				if (inst->asset)
				{
					inst->asset->DeleteAsset(inst);

					int index = 0;
					for (auto comp : inst->components)
					{
						for (auto asset_comp : inst->asset->components)
						{
							if (StringUtils::IsEqual(comp->class_name, ((SceneAssetComp*)asset_comp)->inst_class_name))
							{
								inst->components.erase(inst->components.begin() + index);
								break;
							}
						}

						index++;
					}
				}

				inst->asset = asset;
				asset->instances.push_back(inst);

				for (auto comp : asset->components)
				{
					SceneObjectInstComp* comp_inst = (SceneObjectInstComp*)inst->AddComponent(((SceneAssetComp*)comp)->inst_class_name);
					comp_inst->asset_comp = comp;
				}

				inst->ApplyProperties();
			}
		}
		else
		{
			SceneObject* inst = asset->CreateInstance();
			ed_scene.GenerateUID(inst, false);

			for (auto comp : asset->components)
			{
				SceneObjectInstComp* comp_inst = (SceneObjectInstComp*)inst->AddComponent(((SceneAssetComp*)comp)->inst_class_name);
				comp_inst->asset_comp = comp;
			}

			inst->item = scene_treeview->AddItem(inst->GetName(), 1, inst, parent, -1, false);
			inst->AddChildsToTree(scene_treeview);
			SelectObject(inst, false);
		}
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
		SceneObject* asset = (SceneObject*)sender->GetItemPtr(item);

		if (!asset_drag_as_inst && asset)
		{
			in_select_object = true;
			assets_treeview->SelectItem(nullptr);
			in_select_object = false;

			SelectObject(asset, (sender == assets_treeview));
		}
	}
	else
	if (sender == assets_treeview)
	{
		SceneObject* asset = (SceneObject*)sender->GetItemPtr(item);

		if (!asset_drag_as_inst && asset)
		{
			in_select_object = true;
			scene_treeview->SelectItem(nullptr);
			in_select_object = false;

			SelectObject(asset, (sender == assets_treeview));
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
			//if (item == selectedObject->item)
			{
				SelectObject(nullptr, false);
			}

			if (sender == assets_treeview)
			{
				SceneAsset* asset = (SceneAsset*)ptr;

				for (auto inst : asset->instances)
				{
					scene_treeview->DeleteItem(inst->item);
					ed_scene.DeleteObject((SceneObject*)inst, false);
				}
			}
			else
			{
				SceneObjectInst* inst = dynamic_cast<SceneObjectInst*>((SceneObject*)ptr);

				if (inst && inst->asset)
				{
					inst->asset->DeleteAsset(inst);
				}
			}

			ed_scene.DeleteObject((SceneObject*)ptr, (sender == assets_treeview));

			sender->SelectItem(nullptr);
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