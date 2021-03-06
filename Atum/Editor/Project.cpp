#include "Project.h"
#include "Editor.h"
#include "LayerEntryWidget.h"
#include "SceneObjects/2D/Sprite.h"
#include "shlobj_core.h"

Project project;

void FillGroupsList(EUIComboBox* cbox, void* owner)
{
	for (auto& group : project.groups)
	{
		cbox->AddItem(group.c_str());
	}
}

void FillLayersList(EUIComboBox* cbox, void* owner)
{
	for (auto& layer : project.layers)
	{
		cbox->AddItem(layer->name.c_str());
	}
}

bool Project::CanRun()
{
	if (start_scene == -1)
	{
		MESSAGE_BOX("Can't start", "Please define a start scene");
	}

	return (start_scene != -1);
}

void Project::Load()
{
	JSONReader reader;

	StringUtils::GetPath(project_name.c_str(), project_path);

	if (reader.Parse(project_name.c_str()))
	{
		reader.Read("start_scene", start_scene);
		reader.Read("export_dir", export_dir);

		while (reader.EnterBlock("scenes"))
		{
			scenes.push_back(new SceneHolder());
			SceneHolder* scn = scenes.back();

			string str;
			reader.Read("path", str);
			scn->SetPath(str.c_str());

			reader.Read("selected_object", scn->selected_object);
			reader.Read("selected_object_asset", scn->selected_object_asset);

			while (reader.EnterBlock("include"))
			{
				string str;
				reader.Read("path", str);

				scn->included_pathes.push_back(str);

				reader.LeaveBlock();
			}

			reader.LeaveBlock();
		}

		for (auto& scn : scenes)
		{
			for (auto& incl : scn->included_pathes)
			{
				for (auto& sub_scn : scenes)
				{
					if (StringUtils::IsEqual(sub_scn->path.c_str(), incl.c_str()))
					{
						scn->included.push_back(sub_scn);

						break;
					}
				}
			}
		}

		string ed_scene;
		reader.Read("selected_scene", ed_scene);

		while (reader.EnterBlock("nodes"))
		{
			nodes.push_back(ProjectNode());
			ProjectNode& node = nodes.back();

			reader.Read("name", node.name);
			reader.Read("type", (int&)node.type);

			reader.LeaveBlock();
		}

		RestoreProjectNodes(nodes);

		{
			int index = FindSceneIndex(ed_scene.c_str());

			if (index != -1)
			{
				SelectScene(scenes[index]);
				editor.project_treeview->SelectItem(scenes[index]->item);
			}
		}

		while (reader.EnterBlock("layers"))
		{
			layers.push_back(new Layer());
			Layer* layer = layers.back();

			reader.Read("name", layer->name);
			reader.Read("state", (int&)layer->state);

			layer->wgt = new LayerEntryWidget();
			layer->wgt->Init(layer);

			reader.LeaveBlock();
		}

		while (reader.EnterBlock("groups"))
		{
			groups.push_back(string());
			string& group = groups.back();

			reader.Read("name", group);

			editor.groups_list->AddItem(group.c_str(), nullptr);

			reader.LeaveBlock();
		}
	}
}

void Project::LoadSceneNodes(JSONReader* reader, vector<SceneNode>& nodes, const char* group)
{
	while (reader->EnterBlock(group))
	{
		nodes.push_back(SceneNode());
		SceneNode& node = nodes.back();

		reader->Read("uid", node.uid);
		reader->Read("name", node.name);
		reader->Read("type", (int&)node.type);

		reader->LeaveBlock();
	}
}

void Project::LoadScene(SceneHolder* holder)
{
	if (holder->scene)
	{
		return;
	}

	for (auto& incl : holder->included)
	{
		LoadScene(incl);
	}

	holder->scene = new Scene();

	holder->scene->Init();
	holder->scene->load_asset_inst = true;

	for (auto& incl : holder->included)
	{
		holder->scene->inc_scenes.push_back(incl->scene);
	}

	char path[1024];
	StringUtils::Printf(path, 1024, "%s%s", project_path, holder->path.c_str());

	StringUtils::Copy(holder->scene->project_scene_path, 512, holder->path.c_str());
	holder->scene->Load(path);

	int len = (int)strlen(path);

	path[len - 2] = 'n';
	path[len - 1] = 't';

	JSONReader reader;
	if (reader.Parse(path))
	{
		LoadSceneNodes(&reader, holder->scene_nodes, "scene_nodes");
		LoadSceneNodes(&reader, holder->assets_nodes, "asset_nodes");
	}
	else
	{
		for (int i = 0; i < holder->scene->GetObjectsCount(false); i++)
		{
			SceneNode node;

			node.type = ItemType::Item;
			node.uid = holder->scene->GetObj(i, false)->GetUID();
			node.name = holder->scene->GetObj(i, false)->GetName();

			holder->scene_nodes.push_back(node);
		}

		SceneNode node;

		node.type = ItemType::EndOfGroup;
		node.uid = -1;

		holder->scene_nodes.push_back(node);

		for (int i = 0; i < holder->scene->GetObjectsCount(true); i++)
		{
			SceneNode node;

			node.type = ItemType::Item;
			node.uid = holder->scene->GetObj(i, false)->GetUID();
			node.name = holder->scene->GetObj(i, true)->GetName();

			holder->assets_nodes.push_back(node);
		}

		holder->assets_nodes.push_back(node);
	}
}

void Project::SaveSceneNodes(JSONWriter* writer, vector<SceneNode>& nodes, const char* group)
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

void Project::Save()
{
	if (select_scene)
	{
		FillSelectedObject(select_scene);

		GrabSceneNodes(select_scene);

		SaveCameraPos(select_scene);
	}

	for (auto& holder : scenes)
	{
		if (!holder->scene)
		{
			continue;
		}

		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", project_path, holder->path.c_str());

		holder->scene->Save(path);

		int len = (int)strlen(path);

		path[len - 2] = 'n';
		path[len - 1] = 't';

		JSONWriter writer;
		writer.Start(path);

		SaveSceneNodes(&writer, holder->scene_nodes, "scene_nodes");
		SaveSceneNodes(&writer, holder->assets_nodes, "asset_nodes");
	}

	nodes.clear();
	GrabProjectNodes(nodes, nullptr);

	JSONWriter writer;
	writer.Start(project_name.c_str());

	writer.Write("export_dir", export_dir.c_str());
	writer.Write("start_scene", start_scene);
	writer.Write("scenes_count", (int)scenes.size());

	writer.StartArray("scenes");

	for (auto& scn : scenes)
	{
		writer.StartBlock(nullptr);

		writer.Write("path", scn->path.c_str());

		writer.Write("selected_object", scn->selected_object);
		writer.Write("selected_object_asset", scn->selected_object_asset);

		writer.StartArray("include");

		for (auto& incl : scn->included) 
		{
			writer.StartBlock(nullptr);

			writer.Write("path", incl->path.c_str());

			writer.FinishBlock();
		}

		writer.FinishArray();

		writer.FinishBlock();
	}

	writer.FinishArray();

	if (select_scene)
	{
		writer.Write("selected_scene", select_scene->path.c_str());
	}
	else
	{
		writer.Write("selected_scene", "");
	}

	writer.StartArray("nodes");

	for (auto& node : nodes)
	{
		writer.StartBlock(nullptr);

		writer.Write("name", node.name.c_str());
		writer.Write("type", node.type);

		writer.FinishBlock();
	}

	writer.FinishArray();

	writer.StartArray("layers");

	for (auto& layer : layers)
	{
		writer.StartBlock(nullptr);

		writer.Write("name", layer->name.c_str());
		writer.Write("state", layer->state);

		writer.FinishBlock();
	}

	writer.FinishArray();

	writer.StartArray("groups");

	for (auto& group : groups)
	{
		writer.StartBlock(nullptr);

		writer.Write("name", group.c_str());

		writer.FinishBlock();
	}

	writer.FinishArray();
}

void Project::RestoreSceneNodes(SceneHolder* holder)
{
	editor.scene_treeview->ClearTree();
	RestoreSceneNodes(editor.scene_treeview, holder,false, nullptr);

	editor.assets_treeview->ClearTree();
	RestoreSceneNodes(editor.assets_treeview, holder, true, nullptr);
}

void Project::RestoreSceneNodes(EUITreeView* treeview, SceneHolder* holder, bool is_asset, void* item)
{
	for (auto& incl : holder->included)
	{
		char name[256];
		StringUtils::Printf(name, 256, "%s (Included)", incl->name.c_str());

		SceneTreeItem* tree_item = new SceneTreeItem();
		tree_item->root = true;
		tree_item->scene = incl->scene;
		tree_item->item = treeview->AddItem(name, 0, tree_item, item, -1, true);

		treeview->SetItemOpen(tree_item->item, true);

		RestoreSceneNodes(treeview, incl, is_asset, tree_item->item);
	}

	vector<SceneNode>& nodes = is_asset ? holder->assets_nodes : holder->scene_nodes;

	if (holder->included.size() > 0)
	{
		SceneTreeItem* tree_item = new SceneTreeItem();
		tree_item->root = true;
		tree_item->scene = holder->scene;
		tree_item->item = treeview->AddItem(holder->name.c_str(), 0, tree_item, item, -1, true);

		treeview->SetItemOpen(tree_item->item, true);

		item = tree_item->item;
	}

	if (nodes.size() > 0)
	{
		int index = 0;
		RestoreSceneNodes(treeview, holder->scene, nodes, index, item, is_asset);
	}

	for (int i = 0; i < holder->scene->GetObjectsCount(is_asset); i++)
	{
		SceneObject* obj = holder->scene->GetObj(i, is_asset);

		if (!obj->item)
		{
			SceneTreeItem* tree_item = new SceneTreeItem();
			tree_item->scene = holder->scene;
			tree_item->object = obj;
			tree_item->item = treeview->AddItem(obj->GetName(), 1, tree_item, item, -1, true);

			obj->AddChildsToTree(treeview);
		}
	}
}

void Project::RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, int& index, void* item, bool is_asset)
{
	SceneNode node = nodes[index];
	index++;

	while (node.type != ItemType::EndOfGroup)
	{
		SceneObject* obj = (node.uid != -1) ? scene->FindByUID(node.uid, 0, is_asset) : nullptr;
		
		if (node.uid == -1 || (node.uid != -1 && obj))
		{
			SceneTreeItem* tree_item = new SceneTreeItem();
			tree_item->scene = scene;
			tree_item->object = obj;

			tree_item->item = treeview->AddItem(node.name.c_str(), (node.type == ItemType::Item) ? 1 : 0, tree_item, item, -1, (node.type != ItemType::Item));

			if (node.uid != -1)
			{
				obj->item = tree_item->item;
				obj->treeview = treeview;
				obj->AddChildsToTree(treeview);
			}
			else
			{
				treeview->SetItemOpen(tree_item->item, node.type == ItemType::OpenedFolder);
				RestoreSceneNodes(treeview, scene, nodes, index, tree_item->item, is_asset);
			}
		}

		node = nodes[index];
		index++;
	}
}

void Project::GrabSceneNodes(SceneHolder* holder)
{
	GrabSceneNodes(editor.scene_treeview, holder, nullptr, false);

	GrabSceneNodes(editor.assets_treeview, holder, nullptr, true);
}

void Project::GrabSceneNodes(EUITreeView* treeview, SceneHolder* holder, void* item, bool is_asset)
{
	if (is_asset)
	{
		holder->assets_nodes.clear();
	}
	else
	{
		holder->scene_nodes.clear();
	}

	if (holder->included.size() == 0)
	{
		GrabSceneNodes(treeview, is_asset ? holder->assets_nodes : holder->scene_nodes, item, is_asset);

		return;
	}

	int count = treeview->GetItemChildCount(item);

	for (int i = 0; i < count; i++)
	{
		void* child = treeview->GetItemChild(item, i);

		string name;
		treeview->GetItemText(child, name);

		if (StringUtils::IsEqual(name.c_str(), holder->name.c_str()))
		{
			GrabSceneNodes(treeview, is_asset ? holder->assets_nodes : holder->scene_nodes, child, is_asset);
			continue;
		}

		for (auto& incl : holder->included)
		{
			char incl_name[256];
			StringUtils::Printf(incl_name, 256, "%s (included)", incl->name.c_str());

			if (StringUtils::IsEqual(name.c_str(), incl_name))
			{
				GrabSceneNodes(treeview, incl, child, is_asset);
				break;
			}
		}
	}
}

void Project::GrabSceneNodes(EUITreeView* treeview, vector<SceneNode>& nodes, void* item, bool is_asset)
{
	int count = treeview->GetItemChildCount(item);

	for (int i = 0; i < count; i++)
	{
		void* child = treeview->GetItemChild(item, i);

		nodes.push_back(SceneNode());
		SceneNode& node = nodes.back();

		treeview->GetItemText(child, node.name);
		SceneTreeItem* tree_item = (SceneTreeItem*)treeview->GetItemPtr(child);

		if (tree_item->object)
		{
			node.type = ItemType::Item;
			node.uid = tree_item->object->GetUID();
		}
		else
		{
			node.type = treeview->IsItemOpened(child) ? ItemType::OpenedFolder : ItemType::Folder;
			GrabSceneNodes(treeview, nodes, child, is_asset);
		}
	}

	nodes.push_back(SceneNode());
	SceneNode& node = nodes.back();
	node.type = ItemType::EndOfGroup;
}

void Project::RestoreProjectNodes(vector<ProjectNode>& nodes)
{
	int index = 0;
	editor.project_treeview->ClearTree();
	RestoreProjectNodes(nodes, index, nullptr);
}

void Project::RestoreProjectNodes(vector<ProjectNode>& nodes, int& index, void* item)
{
	ProjectNode& node = nodes[index];
	index++;

	while (node.type != ItemType::EndOfGroup)
	{
		char name[256];
		StringUtils::Printf(name, 256, "%s.sca", node.name.c_str());

		int index_node = (node.type == ItemType::Item) ? FindSceneIndex(name) : -1;

		int image = (node.type == ItemType::Item) ? 1 : 0;

		if (node.type == ItemType::Item && index_node == start_scene)
		{
			image = 2;
		}

		ProjectTreeItem* tree_item = nullptr;
		
		if (index_node != -1)
		{
			tree_item = new ProjectTreeItem();
			tree_item->scene = scenes[index_node];
		}

		void* child = editor.project_treeview->AddItem(node.name.c_str(), image, tree_item, item, -1, true);

		if (index_node != -1)
		{
			tree_item->item = child;
			scenes[index_node]->item = child;

			for (auto incl : scenes[index_node]->included)
			{
				ProjectTreeItem* incl_tree_item = new ProjectTreeItem();
				incl_tree_item->parent = scenes[index_node];
				incl_tree_item->scene = incl;

				incl_tree_item->item = editor.project_treeview->AddItem(incl->name.c_str(), image, incl_tree_item, child, -1, true);
			}
		}

		if (node.type != ItemType::Item)
		{
			editor.project_treeview->SetItemOpen(child, node.type == ItemType::OpenedFolder);
			RestoreProjectNodes(nodes, index, child);
		}

		node = nodes[index];
		index++;
	}
}

void Project::GrabProjectNodes(vector<ProjectNode>& nodes, void* item)
{
	int count = editor.project_treeview->GetItemChildCount(item);

	for (int i = 0; i < count; i++)
	{
		void* child = editor.project_treeview->GetItemChild(item, i);

		nodes.push_back(ProjectNode());
		ProjectNode& node = nodes.back();

		editor.project_treeview->GetItemText(child, node.name);
		void* obj = editor.project_treeview->GetItemPtr(child);

		if (obj)
		{
			node.type = ItemType::Item;
		}
		else
		{
			node.type = editor.project_treeview->IsItemOpened(child) ? ItemType::OpenedFolder : ItemType::Folder;
			GrabProjectNodes(nodes, child);
		}
	}

	nodes.push_back(ProjectNode());
	ProjectNode& node = nodes.back();
	node.type = ItemType::EndOfGroup;
}

void Project::FillSelectedObject(SceneHolder* holder)
{
	if (editor.selectedObject)
	{
		holder->selected_object = editor.selectedObject->GetUID();
		holder->selected_object_asset = editor.isSelectedAsset;
	}
	else
	{
		holder->selected_object = -1;
	}
}

void Project::SetStartScene(const char* path)
{
	if (start_scene != -1)
	{
		editor.project_treeview->SetItemImage(scenes[start_scene]->item, 1);
		start_scene = -1;
	}

	int index = FindSceneIndex(path);

	if (index != -1)
	{
		start_scene = index;
		editor.project_treeview->SetItemImage(scenes[index]->item, 2);
	}
}

void Project::SelectScene(SceneHolder* holder)
{
	if (holder == select_scene)
	{
		return;
	}

	if (select_scene)
	{
		SaveCameraPos(select_scene);

		editor.zoom_ed->SetText((int)(100.0f * Sprite::ed_cam_zoom));

		FillSelectedObject(select_scene);
		editor.SelectObject(nullptr, false);

		GrabSceneNodes(select_scene);

		editor.scene_treeview->ClearTree();
		editor.assets_treeview->ClearTree();

		EnableScene(select_scene, false);
	}

	select_scene = holder;

	if (select_scene)
	{
		if (!select_scene->scene)
		{
			LoadScene(select_scene);
		}

		RestoreSceneNodes(select_scene);

		editor.freecamera.angles = select_scene->scene->camera3d_angles;
		editor.freecamera.pos = select_scene->scene->camera3d_pos;
		Sprite::ed_cam_pos = select_scene->scene->camera2d_pos;
		Sprite::ed_cam_zoom = select_scene->scene->camera2d_zoom;

		editor.moveModeBtn->SetPushed(select_scene->scene->move_mode == 1);
		editor.x_align->SetText(select_scene->scene->gizmo2d_align_x);
		editor.y_align->SetText(select_scene->scene->gizmo2d_align_y);
		editor.gizmo.align2d = Vector2((float)select_scene->scene->gizmo2d_align_x, (float)select_scene->scene->gizmo2d_align_x);

		EnableScene(select_scene, true);

		if (select_scene->selected_object != -1)
		{
			editor.SelectObject(select_scene->scene->FindByUID(select_scene->selected_object, 0, select_scene->selected_object_asset), select_scene->selected_object_asset);
		}
	}

	editor.scene_sheet->Enable(select_scene != nullptr);
}

int Project::FindSceneIndex(const char* path)
{
	char name[256];
	StringUtils::GetFileName(path, name);
	StringUtils::RemoveExtension(name);

	for (int i = 0; i<scenes.size(); i++)
	{
		if (StringUtils::IsEqual(scenes[i]->name.c_str(), name))
		{
			return i;
		}
	}

	return -1;
}

Scene* Project::GetScene(const char* path)
{
	int index = FindSceneIndex(path);

	if (index != -1)
	{
		if (!scenes[index]->scene)
		{
			LoadScene(scenes[index]);
			EnableScene(scenes[index], false);
		}

		return scenes[index]->scene;
	}

	return nullptr;
}

void Project::AddScene(const char* path, void* parent_item)
{
	if (project_path[0] == 0)
	{
		MESSAGE_BOX("Can't add a scene", "Please save project before adding any scene")
		return;
	}

	char cropped_path[1024];
	StringUtils::GetCropPath(project_path, path, cropped_path, 1024);

	if (FindSceneIndex(cropped_path) != -1)
	{
		return;
	}

	SceneHolder* holder = new SceneHolder();
	holder->SetPath(cropped_path);

	LoadScene(holder);

	if (holder->uid == 0)
	{
		GenerateUID(holder);
	}
	else
	{
		for (auto& scn : scenes)
		{
			if (scn == holder)
			{
				continue;
			}

			if (scn->uid == holder->uid)
			{
				holder->scene->Clear();
				delete holder->scene;
				delete holder;

				return;
			}
		}
	}

	holder->uid = holder->scene->uid;

	scenes.push_back(holder);

	char name[256];
	StringUtils::GetFileName(cropped_path, name);
	StringUtils::RemoveExtension(name);

	ProjectTreeItem* treeitem = new ProjectTreeItem();
	treeitem->scene = holder;

	holder->SetPath(cropped_path);
	holder->item = editor.project_treeview->AddItem(name, 1, treeitem, parent_item, -1, true);
	treeitem->item = holder->item;

	SelectScene(holder);
	editor.project_treeview->SelectItem(holder->item);
}

void Project::DeleteScene(SceneHolder* holder)
{
	int index = FindSceneIndex(holder->path.c_str());

	if (index != -1)
	{
		if (start_scene == index)
		{
			start_scene = -1;
		}

		if (select_scene == scenes[index])
		{
			bool prev = editor.allow_delete_objects_by_tree;
			editor.allow_delete_objects_by_tree = false;

			SelectScene(nullptr);

			editor.allow_delete_objects_by_tree = prev;
		}

		delete scenes[index]->scene;
		delete scenes[index];
		scenes.erase(scenes.begin() + index);
	}
}

int Project::FindLayer(const char* layer_name)
{
	for (int index = 0; index < layers.size(); index++)
	{
		if (StringUtils::IsEqual(layers[index]->name.c_str(), layer_name))
		{
			return index;
		}
	}

	return -1;
}

void Project::AddLayer(const char* layer_name)
{
	if (!layer_name[0])
	{
		return;
	}

	if (FindLayer(layer_name) != -1)
	{
		return;
	}

	layers.push_back(new Layer());
	Layer* layer = layers.back();

	layer->name = layer_name;

	layer->wgt = new LayerEntryWidget();
	layer->wgt->Init(layer);
}

void Project::DeleteLayer(Layer* layer)
{
	if (!layer)
	{
		return;
	}

	for (int i = 0; i<layers.size(); i++)
	{
		if (layers[i] == layer)
		{
			delete layers[i];
			layers.erase(layers.begin() + i);
			break;
		}
	}
}

bool Project::LayerHiden(const char* layer_name)
{
	int index = FindLayer(layer_name);

	if (index == -1)
	{
		return false;
	}

	return layers[index]->state == Layer::Invisible;
}

bool Project::LayerSelectable(const char* layer_name)
{
	int index = FindLayer(layer_name);

	if (index == -1)
	{
		return true;
	}

	return layers[index]->state == Layer::Normal;
}

void Project::AddGroup(const char* group_name)
{
	if (!group_name[0])
	{
		return;
	}

	for (auto& group : groups)
	{
		if (StringUtils::IsEqual(group.c_str(), group_name))
		{
			return;
		}
	}

	groups.push_back(group_name);
	editor.groups_list->AddItem(group_name, nullptr);
}

void Project::DeleteGroup(const char* group)
{
	if (!group)
	{
		return;
	}

	for (int i=0; i<groups.size();i++)
	{
		if (StringUtils::IsEqual(groups[i].c_str(), group))
		{
			editor.groups_list->DeleteItemByText(group);
			groups.erase(groups.begin() + i);
			break;
		}
	}
}

bool Project::HaveDepenecy(const char* str, Project::SceneHolder* holder)
{
	if (StringUtils::IsEqual(holder->path.c_str(), str))
	{
		return true;
	}

	for (auto& incl : holder->included)
	{
		bool res = HaveDepenecy(str, incl);

		if (res)
		{
			return true;
		}
	}

	return false;
}

void Project::EnableScene(SceneHolder* holder, bool enable)
{
	for (auto& incl : holder->included)
	{
		EnableScene(incl, enable);
	}

	holder->scene->EnableTasks(enable);
}

void Project::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir, SceneHolder* holder, vector<SceneObject*>& tmp_under_selection)
{
	for (auto& obj : holder->scene->objects)
	{
		if (obj->GetState() != SceneObject::State::Active)
		{
			continue;
		}

		if (!LayerSelectable(obj->layer_name.c_str()))
		{
			continue;
		}

		if (obj->CheckSelection(ms, start, dir))
		{
			tmp_under_selection.push_back(obj);
		}
	}

	for (auto& incl : holder->included)
	{
		CheckSelection(ms, start, dir, incl, tmp_under_selection);
	}
}

SceneObject* Project::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	vector<SceneObject*> tmp_under_selection;

	CheckSelection(ms, start, dir, select_scene, tmp_under_selection);

	bool same_selection = true;

	if (tmp_under_selection.size() != under_selection.size())
	{
		same_selection = false;
	}
	else
	{
		for (int index = 0; index < under_selection.size(); index++)
		{
			if (under_selection[index] != tmp_under_selection[index])
			{
				same_selection = false;
				break;
			}
		}
	}

	if (same_selection)
	{
		under_selection_index++;

		if (under_selection_index >= under_selection.size())
		{
			under_selection_index = 0;
		}
	}
	else
	{
		under_selection_index = 0;
		under_selection = tmp_under_selection;
	}

	return under_selection.size() > 0 ? under_selection[under_selection_index] : nullptr;
}

void Project::SetScene(EUITreeView* treeview, void* item, Scene* scene)
{
	SceneTreeItem* tree_item = (SceneTreeItem*)treeview->GetItemPtr(item);

	if (tree_item->object)
	{
		tree_item->object->SetScene(scene);
	}
	else
	{
		int count = treeview->GetItemChildCount(item);

		for (int i = 0; i < count; i++)
		{
			SetScene(treeview, treeview->GetItemChild(item, i), scene);
		}
	}
}

void Project::GenerateUID(SceneHolder* holder)
{
	uint16_t uid = 0;

	while (uid == 0)
	{
		float koef = Math::Rand() * 0.99f;
		uid = (uint16_t)(koef * 4096) << 4;

		for (auto& scn : scenes)
		{
			if (scn == holder)
			{
				continue;
			}

			if (scn->uid == uid)
			{
				uid = 0;
				break;
			}
		}

		if (uid != 0)
		{
			holder->uid = uid;
			holder->scene->uid = uid;
		}
	}
}

bool Project::OnTreeViewItemDragged(void* item, void* parent)
{
	ProjectTreeItem* parent_item = (ProjectTreeItem*)editor.project_treeview->GetItemPtr(parent);
	ProjectTreeItem* item_item = (ProjectTreeItem*)editor.project_treeview->GetItemPtr(item);

	if (!parent_item && (!item_item || (item_item && !item_item->parent)))
	{
		return true;
	}

	if (parent_item && item_item)
	{
		SceneHolder* parent_holder = parent_item->parent ? parent_item->parent : parent_item->scene;

		if (!HaveDepenecy(item_item->scene->path.c_str(), parent_holder) && !HaveDepenecy(parent_holder->path.c_str(), item_item->scene))
		{
			ProjectTreeItem* holder = new ProjectTreeItem();
			holder->parent = parent_holder;
			holder->scene = item_item->scene;

			parent_holder->included.push_back(holder->scene);

			if (select_scene == parent_holder)
			{
				SelectScene(nullptr);
				SelectScene(parent_holder);
			}

			holder->item = editor.project_treeview->AddItem(holder->scene->name.c_str(), 1, holder, parent_holder->item, -1, true);
		}
	}

	return false;
}

void Project::ProcessPopup(int id, void* popup_item)
{
	ProjectTreeItem* ptr = (ProjectTreeItem*)editor.project_treeview->GetItemPtr(popup_item);

	if (id == 4503)
	{
		if (ptr)
		{
			Project::SceneHolder* scn = ptr->parent ? ptr->parent : ptr->scene;

			SetStartScene(scn->path.c_str());
		}

		return;
	}

	if (id == 4504)
	{
		editor.allow_delete_objects_by_tree = true;
		editor.project_treeview->DeleteItem(popup_item);
		editor.allow_delete_objects_by_tree = false;

		return;
	}

	if (ptr)
	{
		if (ptr->parent)
		{
			popup_item = editor.project_treeview->GetItemParent(ptr->parent->item);
		}
		else
		{
			popup_item = editor.project_treeview->GetItemParent(ptr->item);
		}
	}

	if (id == 4500)
	{
		editor.project_treeview->AddItem("Folder", 0, nullptr, popup_item, -1, true);
	}

	if (id == 4501)
	{
		const char* fileName = EUI::OpenSaveDialog(editor.mainWnd->GetNative(), "Scene file", "sca");

		if (fileName)
		{
			if (popup_item && editor.project_treeview->GetItemPtr(popup_item))
			{
				popup_item = editor.project_treeview->GetItemParent(popup_item);
			}

			AddScene(fileName, popup_item);
		}
	}

	if (id == 4502)
	{
		const char* fileName = EUI::OpenOpenDialog(editor.mainWnd->GetNative(), "Scene file", "sca");

		if (fileName)
		{
			if (popup_item && editor.project_treeview->GetItemPtr(popup_item))
			{
				popup_item = editor.project_treeview->GetItemParent(popup_item);
			}

			AddScene(fileName, popup_item);
		}
	}
}

void Project::OnTreeViewSelChange(void* ptr)
{
	ProjectTreeItem* scn = (ProjectTreeItem*)ptr;

	if (scn)
	{
		SelectScene(scn->parent ? scn->parent : scn->scene);
	}
}

void Project::DeleteItem(void* ptr)
{
	ProjectTreeItem* tree_item = (ProjectTreeItem*)ptr;

	if (tree_item->parent)
	{
		int index = 0;
		for (auto& incl : tree_item->parent->included)
		{
			if (StringUtils::IsEqual(incl->path.c_str(), tree_item->scene->path.c_str()))
			{
				if (select_scene == tree_item->parent)
				{
					bool prev = editor.allow_delete_objects_by_tree;
					editor.allow_delete_objects_by_tree = false;

					SelectScene(nullptr);

					editor.allow_delete_objects_by_tree = prev;
				}

				tree_item->parent->included.erase(tree_item->parent->included.begin() + index);

				if (select_scene == tree_item->parent)
				{
					SelectScene(tree_item->parent);
				}

				break;
			}

			index++;
		}
	}
	else
	{
		DeleteScene(tree_item->scene);
	}
}

void Project::Reset()
{
	select_scene = nullptr;

	for (auto& scn : scenes)
	{
		if (scn->scene)
		{
			scn->scene->Clear();
			delete scn->scene;
			delete scn;
		}
	}

	scenes.clear();

	for (auto& layer : layers)
	{
		delete layer->wgt;
		delete layer;
	}

	project_name.clear();
	export_dir.clear();
	start_scene = -1;
	nodes.clear();
	groups.clear();
	layers.clear();
}

void Project::SelectExportDir()
{
	BROWSEINFO bi = { 0 };

	bi.lpszTitle = "Browse for Folder";

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != NULL)
	{
		TCHAR tszPath[MAX_PATH] = "\0";

		if (SHGetPathFromIDList(pidl, tszPath) == TRUE)
		{
			export_dir = tszPath;
		}

		CoTaskMemFree(pidl);
	}
}

void Project::Export()
{
	if (project_name.empty())
	{
		return;
	}

	if (export_dir.empty())
	{
		SelectExportDir();

		if (export_dir.empty())
		{
			return;
		}
	}

	core.scripts.Start();

	for (auto* holder : scenes)
	{
		if (!holder->scene)
		{
			LoadScene(holder);
		}

		holder->scene->Export();

		EnableScene(holder, false);
	}

	if (select_scene)
	{
		EnableScene(select_scene, true);
	}

	core.scripts.Stop();

	Save();

	core.files.DeleteFolder(export_dir.c_str());
	core.files.CreateFolder((export_dir + "/dummy").c_str());

	DWORD dwAttrib = GetFileAttributes(export_dir.c_str());

	if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		string str = string("Failure in exporing resources to folder:\n") + export_dir;
		MessageBox(nullptr, str.c_str(), "Export failed", MB_ICONERROR);

		return;
	}

	{
		string dest_path = export_dir + "/project";
		core.files.CopyFolder(project_path, dest_path.c_str());
	}

	{
		char project_file_name[512];
		StringUtils::GetFileName(project_name.c_str(), project_file_name);

		string original_name = export_dir + "/project/" + project_file_name;
		string new_name = export_dir + "/project/project.pra";

		rename(original_name.c_str(), new_name.c_str());
	}

	char ApplicationDir[512];
	GetCurrentDirectory(512, ApplicationDir);

	{
		string src_path = string(ApplicationDir) + "/Shaders/GLES";
		string dest_path = export_dir + "/Shaders/GLES";
		core.files.CopyFolder(src_path.c_str(), dest_path.c_str());
	}

	{
		string src_path = string(ApplicationDir) + "/settings";
		string dest_path = export_dir + "/settings";
		core.files.CopyFolder(src_path.c_str(), dest_path.c_str());
	}

	{
		string dest_path = export_dir + "/settings/editor";
		core.files.DeleteFolder(dest_path.c_str());
	}

	{
		string dest_path = export_dir + "/settings/EUI";
		core.files.DeleteFolder(dest_path.c_str());
	}

	string str = string("Resources of project were exported to folder:\n") + export_dir;
	MessageBox(nullptr, str.c_str(), "Export finished", 0);
}

void Project::ShowSettings()
{
	if (project_name.empty())
	{
		return;
	}

	wnd_settings = new EUIWindow("Atum", "settings//editor//icon.ico", EUIWindow::PopupWithCloseBtn, true, 100, 100, 800, 600);
	wnd_settings->SetListener(-1, this, 0);

	EUIPanel* tool_panel = new EUIPanel(wnd_settings, 0, 0, 800, 600);

	EUILabel* export_label = new EUILabel(tool_panel, "Export Directory", 5, 5, 95, 20);
	export_btn = new EUIButton(tool_panel, export_dir.c_str(), 100, 5, 200, 20);
	export_btn->SetListener(-1, this, 0);

	wnd_settings->Show(true);
}

void Project::OnWinClose(EUIWidget* sender)
{
	if (wnd_settings && wnd_settings == sender)
	{
		wnd_settings->Close();
		wnd_settings = nullptr;
	}
}

void Project::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == export_btn)
	{
		SelectExportDir();
		export_btn->SetText(export_dir.c_str());
	}
}

void Project::SaveCameraPos(SceneHolder* holder)
{
	if (editor.selectedObject && editor.selectedObject->UsingOwnCamera())
	{
		if (editor.selectedObject->Is3DObject())
		{
			editor.selectedObject->camera3d_angles = editor.freecamera.angles;
			editor.selectedObject->camera3d_pos = editor.freecamera.pos;
		}
		else
		{
			editor.selectedObject->camera2d_pos = Sprite::ed_cam_pos;
			editor.selectedObject->camera2d_zoom = Sprite::ed_cam_zoom;
		}
	}
	else
	{
		holder->scene->camera2d_pos = Sprite::ed_cam_pos;
		holder->scene->camera2d_zoom = Sprite::ed_cam_zoom;

		holder->scene->camera3d_angles = editor.freecamera.angles;
		holder->scene->camera3d_pos = editor.freecamera.pos;
	}
}