
#pragma once

#include "Scene.h"

class SceneManager : public Object
{
	struct SceneHolder
	{
		string path;
		Scene* scene = nullptr;
		int ref_counter = 0;

		vector<SceneHolder*> included;
		vector<string> included_pathes;
	};

	vector<SceneHolder> scenes;
	map<std::string, SceneHolder*> scenes_search;

	void LoadScene(SceneHolder* holder);
	void UnloadScene(SceneHolder* holder);

public:

	void Init();
	void LoadProject(const char* project_name);
	void LoadScene(const char* name);
	void Execute(float dt);
	void UnloadScene(const char* name);
	void UnloadAll();
};

extern SceneManager scene_manager;