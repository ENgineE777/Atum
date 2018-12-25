
#pragma once

#include "EUI.h"
#include "Support/Support.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/SceneObject.h"

class Project
{
public:
	struct ProjectNode
	{
		int type = 0;
		string name;
	};

	struct Layer
	{
		enum State
		{
			Normal,
			Unselected,
			Invisible
		};

		State state;
		string name;
	};

	struct SceneNode
	{
		int type = 0;
		string name;
		uint32_t uid = -1;
	};

	struct SceneHolder
	{
		string path;
		Scene* scene = nullptr;
		void* item = nullptr;

		vector<SceneNode> scene_nodes;
		vector<SceneNode> assets_nodes;
	};

	Scene* select_scene = nullptr;

	string project_name;
	string start_scene;
	vector<SceneHolder*> scenes;
	vector<ProjectNode> nodes;
	vector<string> groups;
	vector<Layer> layers;

	bool CanRun();
	void Load();
	void LoadSceneNodes(JSONReader* reader, vector<SceneNode>& nodes, const char* group);
	void LoadScene(SceneHolder* holder);
	void SaveSceneNodes(JSONWriter* writer, vector<SceneNode>& nodes, const char* group);
	void Save();

	void RestoreSceneNodes(SceneHolder* holder);
	void RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, bool is_asset);
	void RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, int& index, void* item, bool is_asset);

	void GrabSceneNodes(SceneHolder* holder);
	void GrabSceneNodes(EUITreeView* treeview, vector<SceneNode>& nodes, void* item, bool is_asset);

	void RestoreProjectNodes(vector<ProjectNode>& nodes);
	void RestoreProjectNodes(vector<ProjectNode>& nodes, int& index, void* item);
	void GrabProjectNodes(vector<ProjectNode>& nodes, void* item);

	void SetStartScene(const char* path);
	void SelectScene(const char* path);
	int  FindScene(const char* path);
	void AddScene(const char* path, void* parent_item);
	void DeleteScene(const char* path);

	void Reset();
};
