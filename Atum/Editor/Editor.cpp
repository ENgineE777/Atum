
#include "Editor.h"
#include "Services/Scene/SceneManager.h"

EUITabPanel* Editor::outputPanels = nullptr;
map<string, EUIListBox*> Editor::output_boxes;

Editor editor;

void Editor::Init()
{
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

	menu->StartSubMenu("Project");

	menu->AddItem(MenuProjectSetting, "Settings");
	menu->AddItem(MenuProjectExport, "Export");

	menu->EndSubMenu();

	menu->AddItem(MenuDoc, "Documentation");

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

	trans2d_control = new EUIPanel(tool_panel, 80, 0, 150, 30);

	EUILabel* label = new EUILabel(trans2d_control, "Zoom", 5, 7, 40, 20);
	zoom_ed = new EUIEditBox(trans2d_control, "100.0", 45, 5, 40, 20, EUIEditBox::InputUInteger);
	zoom_ed->SetListener(-1, this, 0);

	trans2d_gizmo = new EUIPanel(tool_panel, 230, 0, 150, 30);

	label = new EUILabel(trans2d_gizmo, "AlignX", 5, 7, 40, 20);
	x_align = new EUIEditBox(trans2d_gizmo, "0", 45, 5, 30, 20, EUIEditBox::InputUInteger);
	x_align->SetListener(-1, this, 0);
	label = new EUILabel(trans2d_gizmo, "AlignY", 82, 7, 40, 20);
	y_align = new EUIEditBox(trans2d_gizmo, "0", 120, 5, 30, 20, EUIEditBox::InputUInteger);
	y_align->SetListener(-1, this, 0);

	object2camera = new EUIButton(trans2d_gizmo, "O=>Cam", 155, 5, 50, 20);
	object2camera->SetListener(-1, this, 0);

	camera2object = new EUIButton(trans2d_gizmo, "Cam=>O", 210, 5, 50, 20);
	camera2object->SetListener(-1, this, 0);

	EUIPanel* main_panel = new EUIPanel(lt_main, 10, 10, 100, 30);

	EUILayout* lt = new EUILayout(main_panel, false);

	EUIPanel* toolsPanel2 = new EUIPanel(lt, 10, 10, 100, 30);
	lt->SetChildSize(toolsPanel2, 250, false);

	EUILayout* leftPanelLt = new EUILayout(toolsPanel2, true);

	EUITabPanel* tabPanel = new EUITabPanel(leftPanelLt, 30, 50, 100, 30);
	tabPanel->SetListener(-1, this, 0);

	EUITabSheet* sheet_prj = tabPanel->AddTab("Project");
	EUILayout* scene_prj = new EUILayout(sheet_prj, true);

	project_treeview = new EUITreeView(scene_prj, 200, 10, 200, 100, true, true);
	project_treeview->SetAutodeletePtr(true);
	project_treeview->SetListener(-1, this, 0);

	project_treeview->AddImage("settings/editor/folder.png");
	project_treeview->AddImage("settings/editor/scene_elem.png");
	project_treeview->AddImage("settings/editor/scene_elem_active.png");

	scene_sheet = tabPanel->AddTab("Scene");
	scene_sheet->Enable(false);

	EUILayout* scene_lt = new EUILayout(scene_sheet, true);

	scene_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	scene_treeview->SetAutodeletePtr(true);
	scene_lt->SetChildSize(scene_treeview, 0.5, true);
	scene_treeview->SetListener(-1, this, 0);

	scene_treeview->AddImage("settings/editor/folder.png");
	scene_treeview->AddImage("settings/editor/scene_elem.png");

	assets_treeview = new EUITreeView(scene_lt, 200, 10, 200, 100, true, true);
	assets_treeview->SetAutodeletePtr(true);
	assets_treeview->SetListener(-1, this, 0);

	assets_treeview->AddImage("settings/editor/folder.png");
	assets_treeview->AddImage("settings/editor/scene_elem.png");

	EUITabSheet* sheet_layers = tabPanel->AddTab("Layers");

	EUILayout* layers_lt = new EUILayout(sheet_layers, true);
	EUIPanel* layers_top_panel = new EUIPanel(layers_lt, 0, 0, 100, 30);

	layers_ebox = new EUIEditBox(layers_top_panel, "", 5, 5, 200, 20, EUIEditBox::InputText);
	layers_ebox->SetListener(-1, this, 0);

	layers_add_btn = new EUIButton(layers_top_panel, "Add", 210, 5, 25, 20);
	layers_add_btn->SetListener(-1, this, 0);

	layers_lt->SetChildSize(layers_top_panel, 30, false);

	layers_cat = new EUICategories(layers_lt, 0, 0, 100, 30);

	EUITabSheet* sheet_groups = tabPanel->AddTab("Groups");

	EUILayout* groups_lt = new EUILayout(sheet_groups, true);

	EUIPanel* groups_top_panel = new EUIPanel(groups_lt, 0, 0, 100, 30);

	groups_ebox = new EUIEditBox(groups_top_panel, "", 5, 5, 165, 20, EUIEditBox::InputText);
	groups_ebox->SetListener(-1, this, 0);

	groups_add_btn = new EUIButton(groups_top_panel, "Add", 175, 5, 25, 20);
	groups_add_btn->SetListener(-1, this, 0);

	groups_del_btn = new EUIButton(groups_top_panel, "Del", 210, 5, 25, 20);
	groups_del_btn->SetListener(-1, this, 0);

	groups_lt->SetChildSize(groups_top_panel, 30, false);

	groups_list = new EUIListBox(groups_lt, 0, 0, 100, 30, true);


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

	obj_cat = new EUICategories(object_lt, 0, 0, 100, 100);

	obj_cmp_wgt.Init(obj_cat);

	AddOutputBox("Output");

	//FIXME
	AddOutputBox("Script");

	core.Init(mainWnd->GetNative(), EUI::GetRenderDevice());

	core.render.AddExecutedLevelPool(1);

	renderTaskPool = core.render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&Editor::Draw);

	freecamera.Init();

	gizmo.Init();
	editor_drawer.Init();

	UpdateGizmoToolbar();

	mainWnd->Show(true);
	mainWnd->Maximaze();

	vp_sheet_lt->SetChildSize(asset_treeview_panel, (float)asset_panel_width, false);
	vp_sheet_lt->Resize();
	ShowVieport();

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
	scene_sheet->Enable(false);
	SelectObject(nullptr, false);
	project.Reset();
	scene_treeview->ClearTree();
	assets_treeview->ClearTree();
	project_treeview->ClearTree();
	layers_cat->DeleteChilds();
	groups_list->ClearList();
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
		if (selectedObject->Is3DObject())
		{
			if (selectedObject->UsingOwnCamera())
			{
				selectedObject->camera3d_pos = freecamera.pos;
				selectedObject->camera3d_angles = freecamera.angles;

				freecamera.pos = project.select_scene->scene->camera3d_pos;
				freecamera.angles = project.select_scene->scene->camera3d_angles;
			}
			else
			{
				project.select_scene->scene->camera3d_pos = freecamera.pos;
				project.select_scene->scene->camera3d_angles = freecamera.angles;
			}
		}
		else
		{
			if (selectedObject->UsingOwnCamera())
			{
				selectedObject->camera2d_pos = Sprite::ed_cam_pos;
				selectedObject->camera2d_zoom = Sprite::ed_cam_zoom;

				Sprite::ed_cam_pos = project.select_scene->scene->camera2d_pos;
				Sprite::ed_cam_zoom = project.select_scene->scene->camera2d_zoom;
			}
			else
			{
				Sprite::ed_cam_pos = selectedObject->camera2d_pos;
				Sprite::ed_cam_zoom = selectedObject->camera2d_zoom;
			}
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
			assets_treeview->SelectItem(nullptr);
		}
		else
		{
			scene_treeview->SelectItem(nullptr);
		}
	}

	Gizmo::inst->Disable();

	selectedObject = obj;
	isSelectedAsset = is_asset;

	if (selectedObject)
	{
		core.files.SetActivePath(selectedObject->GetScene());

		obj->ShowPropWidgets(obj_cat);
		obj->EnableTasks(true);
		obj->SetEditMode(true);

		if (selectedObject->UsingOwnCamera())
		{
			if (selectedObject->Is3DObject())
			{
				freecamera.pos = selectedObject->camera3d_pos;
				freecamera.angles = selectedObject->camera3d_angles;
			}
			else
			{
				Sprite::ed_cam_pos = selectedObject->camera2d_pos;
				Sprite::ed_cam_zoom = selectedObject->camera2d_zoom;
			}
		}

		int panel_width = 1;

		if (isSelectedAsset)
		{
			assets_treeview->SelectItem(obj->item);

			asset_treeview->ClearTree();
			panel_width = ((SceneAsset*)selectedObject)->UseAseetsTree() ? 200 : 1;

			if (panel_width > 1)
			{
				((SceneAsset*)selectedObject)->PreapreAssetTree();
			}
		}
		else
		{
			scene_treeview->SelectItem(obj->item);
		}

		if (asset_panel_width != panel_width)
		{
			asset_panel_width = panel_width;
			vp_sheet_lt->SetChildSize(asset_treeview_panel, (float)panel_width, false);
			vp_sheet_lt->Resize();
			ShowVieport();
		}
	}

	editor.zoom_ed->SetText((int)(100.0f * Sprite::ed_cam_zoom));

	MetaData::scene = selectedObject ? selectedObject->GetScene() : nullptr;

	obj_cmp_wgt.Prepare(selectedObject);

	in_select_object = false;
}

void Editor::CopyObject(SceneObject* obj, void* parent, bool is_asset)
{
	if (!obj)
	{
		return;
	}

	EUITreeView* treeview = is_asset ? assets_treeview : scene_treeview;
	Scene* scene = project.select_scene->scene;

	if (parent)
	{
		scene = ((Project::SceneTreeItem*)treeview->GetItemPtr(parent))->scene;
	}

	SceneObject* copy = scene->CreateObject(obj->class_name, is_asset);
	scene->GenerateUID(copy, is_asset);
	copy->Copy(obj);

	string name = obj->GetName();
	name += "_copy";

	copy->SetName(name.c_str());

	Project::SceneTreeItem* inst_item = new Project::SceneTreeItem();

	inst_item->scene = scene;
	inst_item->object = copy;
	inst_item->item = treeview->AddItem(copy->GetName(), 1, inst_item, parent, -1, false);

	copy->item = inst_item->item;
	copy->treeview = treeview;
	treeview->SelectItem(copy->item);

	copy->AddChildsToTree(treeview);
}

void Editor::CreateSceneObject(const char* name, void* parent, bool is_asset)
{
	EUITreeView* treeview = is_asset ? assets_treeview : scene_treeview;
	Scene* scene = project.select_scene->scene;

	if (parent)
	{
		scene = ((Project::SceneTreeItem*)treeview->GetItemPtr(parent))->scene;
	}
	else
	{
		if (project.select_scene->included.size() > 0)
		{
			return;
		}
	}

	SceneObject* obj = scene->CreateObject(name, is_asset);
	scene->GenerateUID(obj, is_asset);
	obj->ApplyProperties();

	if (obj->Is3DObject())
	{
		Matrix* mat = obj->Trans();
		if (mat)
		{
			mat->Move(freecamera.pos + Vector(cosf(freecamera.angles.x), sinf(freecamera.angles.y), sinf(freecamera.angles.x)) * 5.0f);
		}
	}
	else
	{
		float scale = 1024.0f / core.render.GetDevice()->GetHeight();
		obj->Set2DPos(Sprite::ed_cam_pos * scale);
	}

	obj->SetName(is_asset ? ClassFactorySceneAsset::Find(name)->GetShortName() : ClassFactorySceneObject::Find(name)->GetShortName());

	Project::SceneTreeItem* inst_item = new Project::SceneTreeItem();

	inst_item->scene = scene;
	inst_item->object = obj;
	inst_item->item = treeview->AddItem(obj->GetName(), 1, inst_item, parent, -1, false);

	obj->item = inst_item->item;
	obj->treeview = treeview;

	SelectObject(obj, obj->IsAsset());
	MoveTrans2DToCamera();
}

void Editor::ShowVieport()
{
	EUIPanel* vp = viewport;

	if (in_scene_run)
	{
		vp = game_viewport;
	}

	core.render.GetDevice()->SetVideoMode((int)vp->GetWidth(), (int)vp->GetHeight(), vp->GetNative());
	vp->SetTexture(core.render.GetDevice()->GetBackBuffer());
}

void Editor::StartScene()
{
	Sprite::use_ed_cam = false;

	if (selectedObject)
	{
		selectedObject->EnableTasks(false);
		selectedObject->ShowPropWidgets(nullptr);
	}

	if (project.select_scene)
	{
		project.EnableScene(project.select_scene, false);
	}

	if (!project.CanRun())
	{
		return;
	}

	if (project.project_name.size() == 0)
	{
		const char* fileName = EUI::OpenSaveDialog(mainWnd->GetNative(), "Project file", "pra");

		if (fileName)
		{
			project.project_name = fileName;
		}
		else
		{
			return;
		}
	}

	project.Save();

	core.scripts.Start();

	core.scene_manager.LoadProject(project.project_name.c_str());

	mainWnd->Enable(false);

	gameWnd = new EUIWindow("Game", "", EUIWindow::PopupWithCloseBtn, true, 0, 0, 1378, 720);
	gameWnd->SetListener(-1, this, 0);

	EUILayout* lt = new EUILayout(gameWnd, false);

	game_viewport = new EUIPanel(lt, 0, 0, 1024, 768);
	game_viewport->SetListener(-1, this, EUIWidget::OnResize | EUIWidget::OnUpdate);

	game_viewport->SetFocused();
	
	gameWnd->Show(true);
	gameWnd->SetAtScreenCenter();

	in_scene_run = true;
}

void Editor::StopScene()
{
	if (!in_scene_run)
	{
		return;
	}

	core.scene_manager.UnloadAll();

	core.scripts.Stop();

	core.sounds.ClearAllSounds();

	if (project.select_scene)
	{
		project.EnableScene(project.select_scene, true);
	}

	if (selectedObject)
	{
		selectedObject->EnableTasks(true);
		selectedObject->ShowPropWidgets(obj_cat);
	}

	Sprite::use_ed_cam = true;

	ShowVieport();

	mainWnd->Enable(true);

	gameWnd = nullptr;
	game_viewport = nullptr;

	in_scene_run = false;
}

void Editor::Draw(float dt)
{
	if (!in_scene_run)
	{
		core.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		if (selectedObject && selectedObject->HasOwnTasks() && !selectedObject->Is3DObject())
		{
			Transform2D trans;
			Sprite::FrameState state;
			Sprite::Draw(editor_drawer.checker_texture, COLOR_WHITE, Matrix(),
			             0.0f, Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()),
			             Vector2(((int)(Sprite::ed_cam_pos.x) % 42) / 42.0f, ((int)(1.0f - Sprite::ed_cam_pos.y) % 42) / 42.0f),
			             Vector2((float)core.render.GetDevice()->GetWidth() / 42.0f, (float)core.render.GetDevice()->GetHeight() / 42.0f), false);
		}
	}
	else
	{
		core.render.GetDevice()->Clear(false , COLOR_GRAY, true, 1.0f);
	}

	core.render.ExecutePool(ExecuteLevels::Camera, dt);
	core.render.ExecutePool(ExecuteLevels::Prepare, dt);
	core.render.ExecutePool(ExecuteLevels::Geometry, dt);
	core.render.ExecutePool(ExecuteLevels::DebugGeometry, dt);
	core.render.ExecutePool(ExecuteLevels::Sprites, dt);
	core.render.ExecutePool(ExecuteLevels::Sprites + 1, dt);
	core.render.ExecutePool(ExecuteLevels::Sprites + 2, dt);
	core.render.ExecutePool(ExecuteLevels::PostProcess, dt);
	core.render.ExecutePool(ExecuteLevels::GUI, dt);
	core.render.ExecutePool(ExecuteLevels::Debug, dt);

	core.render.GetDevice()->Present();
}

void Editor::MoveTrans2DToCamera()
{
	float scale = 1024.0f / core.render.GetDevice()->GetHeight();

	Vector2 pos2d = Sprite::ed_cam_pos * scale;

	gizmo.SetTrans2D(pos2d);
}

void Editor::CreatePopup(EUITreeView* treeview, int x, int y, bool is_asset)
{
	if (popup_scene_item && popup_scene_item->AddedToTreeByParent())
	{
		return;
	}

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

void Editor::ProcessTreeviewPopup(EUITreeView* treeview, int id, bool is_asset)
{
	void* parent_item = popup_item;

	if (popup_scene_item)
	{
		parent_item = popup_item ? treeview->GetItemParent(popup_item) : nullptr;
	}

	bool allow_root = true;
	
	if (!parent_item)
	{
		allow_root = project.select_scene->included.size() == 0;
	}

	if (id == 3500 && allow_root)
	{
		Project::SceneTreeItem* tree_item = new Project::SceneTreeItem();
		tree_item->scene = parent_item ? ((Project::SceneTreeItem*)treeview->GetItemPtr(parent_item))->scene : project.select_scene->scene;

		treeview->AddItem("Folder", 0, tree_item, parent_item, -1, true);
	}

	if (id == 3502 && popup_scene_item && allow_root)
	{
		CopyObject(popup_scene_item, popup_scene_item ? treeview->GetItemParent(popup_scene_item->item) : nullptr, is_asset);
	}

	if (id == 3503 && popup_scene_item)
	{
		object_to_copy = popup_scene_item;
	}

	if (id == 3505 && object_to_copy && allow_root)
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
}

SceneObject* Editor::GetSceneObjectFromItem(EUITreeView* tree_view, void* item)
{
	Project::SceneTreeItem* tree_item = item ? (Project::SceneTreeItem*)tree_view->GetItemPtr(item) : nullptr;
	return tree_item ? tree_item->object : nullptr;
}

void Editor::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender == viewport || sender == game_viewport)
	{
		core.controls.OverrideMousePos(mx, my);

		Vector2 ms((float)mx, (float)my);

		if (core.controls.DebugKeyPressed("MS_BTN1", Controls::Active) && core.controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active))
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

			if (selectedObject && selectedObject->Is3DObject() && !isSelectedAsset && !in_scene_run)
			{
				if (allowCopy && core.controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active))
				{
					allowCopy = false;
					//CopyObject(selectedObject, scene_treeview->GetItemParent(selectedObject->item), false);
				}
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
		Vector2 screen_pos = Vector2((float)mx / (float)core.render.GetDevice()->GetWidth(), (float)my / (float)core.render.GetDevice()->GetHeight());

		Vector v;
		v.x = (2.0f * screen_pos.x - 1) / freecamera.proj._11;
		v.y = -(2.0f * screen_pos.y - 1) / freecamera.proj._22;
		v.z = 1.0f;

		Matrix inv_view = freecamera.view;
		inv_view.Inverse();

		Vector dir;
		dir.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		dir.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		dir.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		dir.Normalize();

		if (core.controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active) && (!isSelectedAsset || !selectedObject))
		{
			if (selectedObject && !selectedObject->IsEditMode())
			{
				for (auto comp : selectedObject->components)
				{
					if (comp->IsEditMode())
					{
						comp->CheckSelection({ (float)mx, (float)my }, inv_view.Pos(), dir);
						break;
					}
				}
			}
			else
			if (!selectedObject || !selectedObject->HasOwnTasks())
			{
				SelectObject(project.CheckSelection({ (float)mx, (float)my }, inv_view.Pos(), dir), false);
			}
		}
		else
		{
			sender->CaptureMouse();

			if (core.controls.DebugKeyPressed("KEY_LALT", Controls::Active))
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
		if (project.select_scene)
		{
			project.select_scene->scene->move_mode = moveModeBtn->IsPushed() ? 1 : 0;
		}

		trans2d_control->Show(moveModeBtn->IsPushed() == 1);
	}

	if (sender == playBtn)
	{
		StartScene();
	}

	if (sender == layers_add_btn)
	{
		project.AddLayer(layers_ebox->GetText());
	}

	if (sender == groups_add_btn)
	{
		project.AddGroup(groups_ebox->GetText());
	}

	if (sender == groups_del_btn)
	{
		project.DeleteGroup(groups_list->GetSelectedItemText());
	}

	if (sender == object2camera && gizmo.IsEnabled())
	{
		MoveTrans2DToCamera();
	}

	if (sender == camera2object && gizmo.IsEnabled())
	{
		float scale = 1024.0f / core.render.GetDevice()->GetHeight();
		Sprite::ed_cam_pos = *gizmo.trans2D.pos / scale;
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
		project.select_scene->scene->gizmo2d_align_x = x_align->GetAsInt();
		gizmo.align2d.x = (float)project.select_scene->scene->gizmo2d_align_x;
	}

	if (sender == y_align)
	{
		project.select_scene->scene->gizmo2d_align_y = y_align->GetAsInt();
		gizmo.align2d.y = (float)project.select_scene->scene->gizmo2d_align_y;
	}

	if (sender == zoom_ed)
	{
		Sprite::ed_cam_zoom = 0.01f * zoom_ed->GetAsFloat();
		Sprite::ed_cam_zoom = Math::Clamp(Sprite::ed_cam_zoom, 0.2f, 2.0f);
		freecamera.prev_ed_zoom = Sprite::ed_cam_zoom;
		zoom_ed->SetText((int)(100.0f * Sprite::ed_cam_zoom));
	}
}

void Editor::OnMenuItem(EUIMenu* sender, int activated_id)
{
	if (activated_id == MenuNewID)
	{
		ClearScene();
	}

	if (activated_id == MenuSaveID && project.project_name.size() != 0)
	{
		project.Save();
	}

	if ((activated_id == MenuSaveID && project.project_name.size() == 0) || activated_id == MenuSaveAsID)
	{
		const char* fileName = EUI::OpenSaveDialog(mainWnd->GetNative(), "Project file", "pra");

		if (fileName)
		{
			//TODO: pathes for all scenes should be corected in case project dir was changed
			project.project_name = fileName;
			project.Save();
		}
	}

	if (activated_id == MenuOpenID)
	{
		const char* fileName = EUI::OpenOpenDialog(mainWnd->GetNative(), "Project file", "pra");

		if (fileName)
		{
			ClearScene();
			project.project_name = fileName;
			project.Load();
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

	if (activated_id == MenuProjectSetting)
	{
		project.ShowSettings();
	}

	if (activated_id == MenuProjectExport)
	{
		project.Export();
	}

	if (activated_id == MenuDoc)
	{
		ShellExecute(NULL, NULL, "chrome.exe", "https://atumengine.com/doc_html/html/index.html", NULL, SW_SHOWNORMAL);
	}

	if (popup_parent == asset_treeview && selectedObject && isSelectedAsset)
	{
		((SceneAsset*)selectedObject)->OnAssetTreePopupItem(activated_id);
	}
	else
	if (popup_parent == project_treeview)
	{
		project.ProcessPopup(activated_id, popup_item);
	}
	else
	if (popup_parent == scene_treeview || popup_parent == assets_treeview)
	{
		ProcessTreeviewPopup((EUITreeView*)popup_parent, activated_id, (popup_parent == assets_treeview));
	}
	else
	if (selectedObject)
	{
		selectedObject->OnPopupMenuItem(activated_id);
	}

	popup_item = nullptr;
	popup_scene_item = nullptr;
}

void Editor::OnUpdate(EUIWidget* sender)
{
	if (in_scene_run && sender != game_viewport)
	{
		return;
	}

	ShowVieport();

	core.CountDeltaTime();
	float dt = core.GetDeltaTime();

	if (!in_scene_run)
	{
		asset_drag_as_inst = core.controls.DebugKeyPressed("KEY_LCONTROL", Controls::Active, true);

		if (project.select_scene)
		{
			freecamera.mode_2d = selectedObject ? !selectedObject->Is3DObject() : (project.select_scene->scene->move_mode == 1);
		}

		if (!freecamera.mode_2d && (!selectedObject || !selectedObject->HasOwnTasks()))
		{
			for (int i = 0; i <= 20; i++)
			{
				float pos = (float)i - 10.0f;

				core.render.DebugLine(Vector(pos, 0.0f, -10.0f), COLOR_WHITE, Vector(pos, 0.0f, 10.0f), COLOR_WHITE);
				core.render.DebugLine(Vector(-10.0f, 0.0f, pos), COLOR_WHITE, Vector(10.0f, 0.0f, pos), COLOR_WHITE);
			}
		}

		if (rect_select)
		{
			core.render.DebugRect2D(prev_ms, ms_rect, COLOR_YELLOW);
		}

		freecamera.Update(dt);

		if (freecamera.mode_2d && fabs(Sprite::ed_cam_zoom - freecamera.prev_ed_zoom) > 0.01f)
		{
			zoom_ed->SetText((int)(100.0f * Sprite::ed_cam_zoom));
		}

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
			core.render.DebugLine2D(Vector2(0.5f, 0.5f + i), color, Vector2((float)core.render.GetDevice()->GetWidth(), 0.5f + i), color);
			core.render.DebugLine2D(Vector2(0.5f, (float)core.render.GetDevice()->GetHeight() - 0.5f - i), color, Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight() - 0.5f - i), color);
			core.render.DebugLine2D(Vector2(0.5f + i, 0.5f), color, Vector2(0.5f + i, (float)core.render.GetDevice()->GetHeight()), color);
			core.render.DebugLine2D(Vector2((float)core.render.GetDevice()->GetWidth() - i - 0.5f, 0.5f), color, Vector2((float)core.render.GetDevice()->GetWidth() - i - 0.5f, (float)core.render.GetDevice()->GetHeight()), color);
		}
	}

	core.controls.SetFocused(in_scene_run ? game_viewport->IsFocused() : viewport->IsFocused());

	core.render.DebugPrintText(5.0f, COLOR_WHITE, "%i", core.GetFPS());

	core.Update();

	if (!in_scene_run)
	{
		if (selectedObject)
		{
			void* sel_item = isSelectedAsset ? assets_treeview->GetSelectedItem() : scene_treeview->GetSelectedItem();

			if (sel_item)
			{
				void* item_ptr = isSelectedAsset ? assets_treeview->GetItemPtr(sel_item) : scene_treeview->GetItemPtr(sel_item);

				if (item_ptr && selectedObject == ((Project::SceneTreeItem*)item_ptr)->object)
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
						selectedObject->SetName(selectedObject->GetName());

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
		}

		if (project.select_scene)
		{
			if (selectedObject && selectedObject->HasOwnTasks())
			{
				project.EnableScene(project.select_scene, false);
				selectedObject->Tasks(true)->Execute(dt);
			}
			else
			{
				project.EnableScene(project.select_scene, true);
				project.select_scene->scene->Execute(dt);
			}
		}
	}
	else
	{
		if (core.scene_manager.failure_on_scene_play)
		{
			MessageBox(nullptr, core.scene_manager.failure_on_scene_play_message.c_str(), "Error on scene play", MB_ICONERROR);
			gameWnd->Close();
			StopScene();

			mainWnd->Show(true);
			mainWnd->Maximaze();
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
	if (sender == project_treeview && sender == target)
	{
		return project.OnTreeViewItemDragged(item, parent);
	}

	if ((sender == assets_treeview || sender == scene_treeview || sender == asset_treeview) && target == viewport)
	{
		if (selectedObject)
		{
			int x, y;
			viewport->GetMousePos(x, y);
			Vector2 ms((float)x, (float)y);

			selectedObject->OnDragObjectFromTreeView(sender == assets_treeview, GetSceneObjectFromItem(sender, item), ms);
		}

		return false;
	}

	if ((sender == assets_treeview || sender == asset_treeview ) && target == asset_treeview && selectedObject && isSelectedAsset)
	{
		return ((SceneAsset*)selectedObject)->OnAssetTreeViewItemDragged((sender == assets_treeview), GetSceneObjectFromItem(sender, item),
		                                                                 prev_child_index, GetSceneObjectFromItem(asset_treeview, parent), child_index);
	}

	if (sender == target)
	{
		Project::SceneTreeItem* with = (Project::SceneTreeItem*)sender->GetItemPtr(item);
		Project::SceneTreeItem* to = (Project::SceneTreeItem*)sender->GetItemPtr(parent);

		if (with->object && to)
		{
			if (with->object->IsAsset())
			{
				SceneAsset* asset = (SceneAsset*)with->object;

				for (auto& inst : asset->instances)
				{
					SceneObject* object = inst.GetObject();

					if (!object->GetScene()->DependFromScene(to->scene))
					{
						return false;
					}
				}
			}
			else
			{
				SceneObjectInst* inst = dynamic_cast<SceneObjectInst*>(with->object);

				if (inst && to && !to->scene->FindByUID(inst->asset->GetUID(), 0, true))
				{
					return false;
				}
			}
		}

		if (with->root)
		{
			return false;
		}

		if (project.select_scene->included.size() > 0)
		{
			if (!to)
			{
				return false;
			}
		}

		if (with->object && with->object->AddedToTreeByParent())
		{
			return false;
		}

		if (to && with->scene != to->scene)
		{
			project.SetScene(sender, item, to->scene);
		}

		return true;
	}

	if (sender == assets_treeview && target == scene_treeview && GetSceneObjectFromItem(assets_treeview, item))
	{
		Project::SceneTreeItem* tree_item = (Project::SceneTreeItem*)assets_treeview->GetItemPtr(item);

		SceneAsset* asset = (SceneAsset*)tree_item->object;

		if (core.controls.DebugKeyPressed("KEY_LSHIFT", Controls::Active, true))
		{
			Project::SceneTreeItem* target_tree_item = (Project::SceneTreeItem*)sender->GetItemPtr(sender->GetItemChild(parent, child_index > 0 ? child_index - 1 : child_index));

			if (target_tree_item->object && StringUtils::IsEqual(asset->inst_class_name, target_tree_item->object->class_name))
			{
				SceneObjectInst* inst = (SceneObjectInst*)target_tree_item->object;
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

				asset->instances.push_back(SceneAsset::AssetInstance(inst));

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
			Project::SceneTreeItem* target_tree_item = (Project::SceneTreeItem*)sender->GetItemPtr(parent);

			Scene* scene = target_tree_item ? target_tree_item->scene : asset->GetScene();

			SceneObject* inst = asset->CreateInstance(scene);

			if (inst)
			{
				for (auto comp : asset->components)
				{
					SceneObjectInstComp* comp_inst = (SceneObjectInstComp*)inst->AddComponent(((SceneAssetComp*)comp)->inst_class_name);
					comp_inst->asset_comp = comp;
				}

				Project::SceneTreeItem* inst_item = new Project::SceneTreeItem();

				inst_item->scene = scene;
				inst_item->object = inst;
				inst_item->item = scene_treeview->AddItem(inst->GetName(), 1, inst_item, parent, -1, false);

				inst->item = inst_item->item;
				inst->treeview = scene_treeview;
				inst->AddChildsToTree(scene_treeview);
				SelectObject(inst, false);

				MoveTrans2DToCamera();
			}
		}
	}

	if ((sender == scene_treeview || sender == assets_treeview) && target->GetUserData())
	{
		SceneObjectWidget* wgt = (SceneObjectWidget*)target->GetUserData();

		Project::SceneTreeItem* with = (Project::SceneTreeItem*)sender->GetItemPtr(item);
		
		wgt->SetObject(with->object);
	}

	return false;
}

void Editor::OnTreeViewSelChange(EUITreeView* sender, void* item)
{
	SceneObject* object = GetSceneObjectFromItem(sender, item);

	if (sender == project_treeview)
	{
		project.OnTreeViewSelChange(asset_treeview->GetItemPtr(item));
	}
	else
	if (sender == asset_treeview && selectedObject && isSelectedAsset)
	{
		if (!asset_drag_as_inst && object)
		{
			((SceneAsset*)selectedObject)->OnAssetTreeSelChange(GetSceneObjectFromItem(asset_treeview, item));
		}
	}
	else
	if (sender == scene_treeview)
	{
		if (!asset_drag_as_inst && object)
		{
			in_select_object = true;
			assets_treeview->SelectItem(nullptr);
			in_select_object = false;

			SelectObject(object, false);
		}
	}
	else
	if (sender == assets_treeview)
	{
		if (!asset_drag_as_inst && object)
		{
			in_select_object = true;
			scene_treeview->SelectItem(nullptr);
			in_select_object = false;

			SelectObject(object, true);
		}
	}
}

void Editor::OnTreeReCreateItem(EUITreeView* sender, void* item, void* ptr)
{
	if (sender == scene_treeview || sender == assets_treeview)
	{
		Project::SceneTreeItem* tree_item = static_cast<Project::SceneTreeItem*>(ptr);

		tree_item->item = item;
		tree_item->object->item = item;
	}
}

void Editor::OnTreeDeleteItem(EUITreeView* sender, void* item, void* ptr)
{
	if (sender == project_treeview && allow_delete_objects_by_tree)
	{
		if (ptr)
		{
			project.DeleteItem(ptr);
		}
	}

	if ((sender == scene_treeview || sender == assets_treeview) && allow_delete_objects_by_tree)
	{
		if (ptr)
		{
			Project::SceneTreeItem* tree_item = static_cast<Project::SceneTreeItem*>(ptr);

			if (tree_item->object)
			{
				if (item == tree_item->item)
				{
					SelectObject(nullptr, false);
				}

				if (sender == assets_treeview)
				{
					SceneAsset* asset = static_cast<SceneAsset*>(tree_item->object);

					for (auto inst : asset->instances)
					{
						scene_treeview->DeleteItem(inst.GetObject()->item);
						inst.GetObject()->GetScene()->DeleteObject(inst.GetObject(), false, true);
					}
				}
				else
				{
					SceneObjectInst* inst = dynamic_cast<SceneObjectInst*>(tree_item->object);

					if (inst && inst->asset)
					{
						inst->asset->DeleteAsset(inst);
					}
				}

				tree_item->scene->DeleteObject(tree_item->object, (sender == assets_treeview), true);

				sender->SelectItem(nullptr);
			}
		}
	}
}

void Editor::OnTreeViewRightClick(EUITreeView* sender, int x, int y, void* item, int child_index)
{
	if (sender == asset_treeview && selectedObject && isSelectedAsset)
	{
		popup_parent = sender;
		((SceneAsset*)selectedObject)->OnAssetTreeRightClick(x, y, GetSceneObjectFromItem(sender, item), child_index);
	}

	if (sender == project_treeview)
	{
		popup_menu->StartMenu(true);

		popup_menu->AddItem(4500, "Create Folder");
		popup_menu->AddItem(4501, "Add New scene");
		popup_menu->AddItem(4502, "Add Scene");
		popup_menu->AddItem(4503, "Mark as start scene");
		popup_menu->AddItem(4504, "Delete");

		popup_menu->EndSubMenu();

		popup_item = item;
		popup_parent = sender;
		popup_menu->ShowAsPopup(project_treeview, x, y);
	}
	else
	if (sender == scene_treeview || sender == assets_treeview)
	{
		popup_item = item;
		Project::SceneTreeItem* tree_item = ((Project::SceneTreeItem*)sender->GetItemPtr(item));
		popup_scene_item = tree_item ? tree_item->object : nullptr;
		popup_child_index = child_index;

		popup_parent = sender;
		CreatePopup(sender, x, y, (sender == assets_treeview));
	}
}

void Editor::OnTreeViewSelItemTextChanged(EUITreeView* sender, void* item, const char* text)
{
	if (sender == scene_treeview || sender == assets_treeview)
	{
		SceneObject* object = GetSceneObjectFromItem(sender, item);

		if (object)
		{
			object->SetName(text);
		}

		sender->SetItemText(item, text);
	}
}