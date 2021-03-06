
#pragma once

#include "EUI.h"
#include "Support/Support.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/SceneObject.h"

/**
\ingroup gr_code_editor
*/

class Project : public EUIWidget::Listener
{
public:

	enum ItemType
	{
		Folder,
		Item,
		EndOfGroup,
		OpenedFolder
	};

	struct ProjectNode
	{
		ItemType type = ItemType::Folder;
		string name;
	};

	struct Layer
	{
		enum State
		{
			Normal,
			Unselectable,
			Invisible
		};

		State state = Normal;
		string name;
		class LayerEntryWidget* wgt;
	};

	struct SceneNode
	{
		ItemType type = ItemType::Folder;
		string name;
		uint32_t uid = -1;
	};

	struct SceneHolder
	{
		string path;
		string name;
		uint16_t uid = 0;
		Scene* scene = nullptr;
		uint32_t selected_object = -1;
		bool  selected_object_asset = false;
		void* item = nullptr;

		vector<SceneHolder*> included;
		vector<SceneNode> scene_nodes;
		vector<SceneNode> assets_nodes;
		vector<string> included_pathes;

		void SetPath(const char* set_path)
		{
			path = set_path;

			char set_name[256];
			StringUtils::GetFileName(path.c_str(), set_name);
			StringUtils::RemoveExtension(set_name);

			name = set_name;
		}
	};

	struct ProjectTreeItem
	{
		SceneHolder* parent = nullptr;
		SceneHolder* scene = nullptr;
		void* item = nullptr;
	};

	struct SceneTreeItem
	{
		Scene* scene = nullptr;
		SceneObject* object = nullptr;
		void* item = nullptr;
		bool root = false;

		SceneTreeItem() = default;
		SceneTreeItem(SceneObject* set_object)
		{
			scene = set_object->scene;
			object = set_object;
		};
	};

	SceneHolder* select_scene = nullptr;

	string project_name;
	string export_dir;
	char project_path[1024];
	int start_scene = -1;
	vector<SceneHolder*> scenes;
	vector<ProjectNode> nodes;
	vector<string> groups;
	vector<Layer*> layers;

	EUIWindow* wnd_settings = nullptr;
	EUIButton* export_btn = nullptr;

	bool CanRun();
	void Load();
	void LoadSceneNodes(JSONReader* reader, vector<SceneNode>& nodes, const char* group);
	void LoadScene(SceneHolder* holder);
	void SaveSceneNodes(JSONWriter* writer, vector<SceneNode>& nodes, const char* group);
	void Save();

	void RestoreSceneNodes(SceneHolder* holder);
	void RestoreSceneNodes(EUITreeView* treeview, SceneHolder* scene, bool is_asset, void* item);
	void RestoreSceneNodes(EUITreeView* treeview, Scene* scene, vector<SceneNode>& nodes, int& index, void* item, bool is_asset);

	void GrabSceneNodes(SceneHolder* holder);
	void GrabSceneNodes(EUITreeView* treeview, SceneHolder* holder, void* item, bool is_asset);
	void GrabSceneNodes(EUITreeView* treeview, vector<SceneNode>& nodes, void* item, bool is_asset);

	void RestoreProjectNodes(vector<ProjectNode>& nodes);
	void RestoreProjectNodes(vector<ProjectNode>& nodes, int& index, void* item);
	void GrabProjectNodes(vector<ProjectNode>& nodes, void* item);

	void FillSelectedObject(SceneHolder* holder);

	void SetStartScene(const char* path);
	void SelectScene(SceneHolder* holder);
	int  FindSceneIndex(const char* path);
	Scene* GetScene(const char* path);
	void AddScene(const char* path, void* parent_item);
	void DeleteScene(SceneHolder* holder);

	int FindLayer(const char* layer);

	void AddLayer(const char* layer);
	void DeleteLayer(Layer* layer);

	bool LayerHiden(const char* layer);
	bool LayerSelectable(const char* layer);

	void AddGroup(const char* group);
	void DeleteGroup(const char* group);

	bool HaveDepenecy(const char* str, SceneHolder* holder);

	void EnableScene(SceneHolder* holder, bool enable);

	int under_selection_index = 0;
	vector<SceneObject*> under_selection;
	void CheckSelection(Vector2 ms, Vector3 start, Vector3 dir, SceneHolder* holder, vector<SceneObject*>& tmp_under_selection);
	SceneObject* CheckSelection(Vector2 ms, Vector3 start, Vector3 dir);
	void SetScene(EUITreeView* treeview, void* item, Scene* scene);
	void GenerateUID(SceneHolder* holder);
	bool OnTreeViewItemDragged(void* item, void* parent);
	void ProcessPopup(int id, void* popup_item);
	void OnTreeViewSelChange(void* ptr);
	void DeleteItem(void* ptr);
	void Reset();

	void Export();
	void ShowSettings();

	void SelectExportDir();
	void OnWinClose(EUIWidget* sender) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;

	void SaveCameraPos(SceneHolder* holder);
};

extern Project project;
