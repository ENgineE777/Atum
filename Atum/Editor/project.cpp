#include "Project.h"
#include "Editor.h"
#include "LayerEntryWidget.h"
#include "SceneObjects/2D/Sprite.h"

Project project;

void FillGroupsList(EUIComboBox* cbox)
{
	for (auto& group : project.groups)
	{
		cbox->AddItem(group.c_str());
	}
}

void FillLayersList(EUIComboBox* cbox)
{
	for (auto& layer : project.layers)
	{
		cbox->AddItem(layer->name.c_str());
	}
}

bool Project::CanRun()
{
	return !start_scene.empty();
}

void Project::Load()
{
	JSONReader reader;
	if (reader.Parse(project_name.c_str()))
	{
		reader.Read("start_scene", start_scene);

		while (reader.EnterBlock("scenes"))
		{
			scenes.push_back(new SceneHolder());
			SceneHolder* scn = scenes.back();

			reader.Read("path", scn->path);

			reader.LeaveBlock();
		}

		while (reader.EnterBlock("nodes"))
		{
			nodes.push_back(ProjectNode());
			ProjectNode& node = nodes.back();

			reader.Read("name", node.name);
			reader.Read("type", node.type);

			reader.LeaveBlock();
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

	RestoreProjectNodes(nodes);
}

void Project::LoadSceneNodes(JSONReader* reader, vector<SceneNode>& nodes, const char* group)
{
	while (reader->EnterBlock(group))
	{
		nodes.push_back(SceneNode());
		SceneNode& node = nodes.back();

		reader->Read("uid", node.uid);
		reader->Read("name", node.name);
		reader->Read("type", node.type);

		reader->LeaveBlock();
	}
}

void Project::LoadScene(SceneHolder* holder)
{
	holder->scene = new Scene();
	holder->scene->Init();
	holder->scene->Load(holder->path.c_str());

	string scene_tree = holder->path;
	scene_tree.resize(scene_tree.size() - 3);
	scene_tree += "snt";

	JSONReader reader;
	if (reader.Parse(scene_tree.c_str()))
	{
		LoadSceneNodes(&reader, holder->scene_nodes, "scene_nodes");
		LoadSceneNodes(&reader, holder->assets_nodes, "asset_nodes");
	}
	else
	{
		for (int i = 0; i < holder->scene->GetObjectsCount(false); i++)
		{
			SceneNode node;

			node.type = 1;
			node.uid = holder->scene->GetObj(i, false)->GetUID();
			node.name = holder->scene->GetObj(i, false)->GetName();

			holder->scene_nodes.push_back(node);
		}

		SceneNode node;

		node.type = 2;
		node.uid = -1;

		holder->scene_nodes.push_back(node);

		for (int i = 0; i < holder->scene->GetObjectsCount(true); i++)
		{
			SceneNode node;

			node.type = 1;
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
	for (auto& scn : scenes)
	{
		if (!scn->scene)
		{
			continue;
		}

		scn->scene->Save(scn->path.c_str());

		if (scn->scene == select_scene)
		{
			GrabSceneNodes(scn);
		}

		string scene_tree = scn->path;
		scene_tree.resize(scene_tree.size() - 3);
		scene_tree += "snt";

		JSONWriter writer;
		writer.Start(scene_tree.c_str());

		SaveSceneNodes(&writer, scn->scene_nodes, "scene_nodes");
		SaveSceneNodes(&writer, scn->assets_nodes, "asset_nodes");
	}

	nodes.clear();
	GrabProjectNodes(nodes, nullptr);

	JSONWriter writer;
	writer.Start(project_name.c_str());

	writer.Write("start_scene", start_scene.c_str());

	writer.StartArray("scenes");

	for (auto& scn : scenes)
	{
		writer.StartBlock(nullptr);

		writer.Write("path", scn->path.c_str());

		writer.FinishBlock();
	}

	writer.FinishArray();

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
	RestoreSceneNodes(editor.scene_treeview, holder->scene, holder->scene_nodes, false);
	RestoreSceneNodes(editor.assets_treeview, holder->scene, holder->assets_nodes, true);
}

void Project::RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, bool is_asset)
{
	treeview->ClearTree();

	if (nodes.size() == 0)
	{
		for (int i = 0; i < scene->GetObjectsCount(is_asset); i++)
		{
			SceneObject* obj = scene->GetObj(i, is_asset);
			obj->item = treeview->AddItem(obj->GetName(), 1, obj, nullptr, -1, false);
			obj->AddChildsToTree(treeview);
		}
	}
	else
	{
		int index = 0;
		RestoreSceneNodes(treeview, scene, nodes, index, nullptr, is_asset);
	}
}

void Project::RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, int& index, void* item, bool is_asset)
{
	SceneNode& node = nodes[index];
	index++;

	while (node.type != 2)
	{
		SceneObject* obj = (node.uid != -1) ? scene->FindByUID(node.uid, 0, is_asset) : nullptr;

		void* child = treeview->AddItem(node.name.c_str(), node.type, obj, item, -1, (node.type == 0));

		if (obj)
		{
			obj->item = child;
			obj->AddChildsToTree(treeview);
		}
		else
		{
			RestoreSceneNodes(treeview, scene, nodes, index, child, is_asset);
		}

		node = nodes[index];
		index++;
	}
}

void Project::GrabSceneNodes(SceneHolder* holder)
{
	holder->scene_nodes.clear();
	GrabSceneNodes(editor.scene_treeview, holder->scene_nodes, nullptr, false);

	holder->assets_nodes.clear();
	GrabSceneNodes(editor.assets_treeview, holder->assets_nodes, nullptr, true);
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
		SceneObject* obj = (SceneObject*)treeview->GetItemPtr(child);

		if (obj)
		{
			node.type = 1;
			node.uid = obj->GetUID();
		}
		else
		{
			GrabSceneNodes(treeview, nodes, child, is_asset);
		}
	}

	nodes.push_back(SceneNode());
	SceneNode& node = nodes.back();
	node.type = 2;
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

	while (node.type != 2)
	{
		int index_node = (node.type == 1) ? FindScene(node.name.c_str()) : -1;

		int image = node.type;

		if (node.type == 1 && StringUtils::IsEqual(node.name.c_str(), start_scene.c_str()))
		{
			image = 2;
		}

		void* child = editor.project_treeview->AddItem(node.name.c_str(), image, index_node != -1 ? scenes[index_node] : nullptr, item, -1, (node.type == 0));

		if (index_node != -1)
		{
			scenes[index_node]->item = child;
		}

		if (node.type != 1)
		{
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
			node.type = 1;
		}
		else
		{
			GrabProjectNodes(nodes, child);
		}
	}

	nodes.push_back(ProjectNode());
	ProjectNode& node = nodes.back();
	node.type = 2;
}

void Project::SetStartScene(const char* path)
{
	int index = FindScene(start_scene.c_str());

	if (index != -1)
	{
		editor.project_treeview->SetItemImage(scenes[index]->item, 1);
		start_scene = "";
	}

	index = FindScene(path);

	if (index != -1)
	{
		start_scene = path;
		editor.project_treeview->SetItemImage(scenes[index]->item, 2);
	}
}

void Project::SelectScene(const char* path)
{
	Scene* prev_select_scene = select_scene;

	int index = FindScene(path);

	if (index != -1)
	{
		if (!scenes[index]->scene)
		{
			LoadScene(scenes[index]);
		}

		select_scene = scenes[index]->scene;
	}
	else
	{
		select_scene = nullptr;
	}

	if (prev_select_scene != select_scene)
	{
		if (prev_select_scene)
		{
			if (editor.selectedObject && editor.selectedObject->UsingCamera2DPos())
			{
				editor.selectedObject->Camera2DPos() = Sprite::ed_cam_pos;
			}
			else
			{
				prev_select_scene->camera2d_pos = Sprite::ed_cam_pos;
			}

			prev_select_scene->camera3d_angles = editor.freecamera.angles;
			prev_select_scene->camera3d_pos = editor.freecamera.pos;

			for (auto& scn : scenes)
			{
				if (scn->scene == prev_select_scene)
				{
					GrabSceneNodes(scn);
					break;
				}
			}

			prev_select_scene->EnableTasks(false);
		}

		if (select_scene)
		{
			RestoreSceneNodes(scenes[index]);

			Sprite::ed_cam_pos = select_scene->camera2d_pos;
			editor.freecamera.angles = select_scene->camera3d_angles;
			editor.freecamera.pos = select_scene->camera3d_pos;
			editor.moveModeBtn->SetPushed(select_scene->move_mode == 1);
			editor.x_align->SetText(select_scene->gizmo2d_align_x);
			editor.y_align->SetText(select_scene->gizmo2d_align_y);
			editor.gizmo.align2d = Vector2((float)select_scene->gizmo2d_align_x, (float)select_scene->gizmo2d_align_x);

			select_scene->EnableTasks(true);
		}
	}
}

int Project::FindScene(const char* path)
{
	for (int i = 0; i<scenes.size(); i++)
	{
		if (StringUtils::IsEqual(scenes[i]->path.c_str(), path))
		{
			return i;
		}
	}

	return -1;
}

void Project::AddScene(const char* path, void* parent_item)
{
	char cur_dir[2048];
	GetCurrentDirectory(512, cur_dir);

	char cropped_path[1024];
	StringUtils::GetCropPath(cur_dir, path, cropped_path, 1024);
	StringUtils::RemoveFirstChar(cropped_path);

	if (FindScene(cropped_path) != -1)
	{
		return;
	}

	scenes.push_back(new SceneHolder());
	SceneHolder* scn = scenes.back();

	scn->path = cropped_path;
	scn->item = editor.project_treeview->AddItem(scn->path.c_str(), 1, scn, parent_item, -1, true);

	if (start_scene.size() == 0)
	{
		start_scene = cropped_path;
	}

	SelectScene(cropped_path);
	editor.project_treeview->SelectItem(scn->item);
}

void Project::DeleteScene(const char* path)
{
	if (StringUtils::IsEqual(path, start_scene.c_str()))
	{
		start_scene.clear();
	}

	int index = FindScene(path);

	if (index != -1)
	{
		if (select_scene == scenes[index]->scene)
		{
			SelectScene("");
		}

		delete scenes[index]->scene;
		delete scenes[index];
		scenes.erase(scenes.begin() + index);

		if (scenes.size() > 0 && start_scene.size() != 0)
		{
			start_scene = scenes[0]->path;
		}
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

	for (auto& layer : layers)
	{
		delete layer->wgt;
		delete layer;
	}

	project_name.clear();
	start_scene.clear();
	nodes.clear();
	groups.clear();
	layers.clear();
}