
#pragma once

#include "Scene.h"
#include "Services/Physic/Physics.h"

class SceneManager : public Object
{
	friend class SceneObject;

	struct SceneHolder
	{
		string path;
		Scene* scene = nullptr;
		int ref_counter = 0;

		vector<SceneHolder*> included;
		vector<string> included_pathes;
	};

	char project_path[1024];

	PhysScene* pscene = nullptr;

	vector<SceneHolder> scenes;

	vector<SceneHolder*> scenes_to_delete;

	map<std::string, SceneHolder*> scenes_search;

	void LoadScene(SceneHolder* holder);
	void UnloadScene(SceneHolder* holder);

public:

	void Init();
	void LoadProject(const char* project_name);
	inline PhysScene* PScene() { return pscene; }
	void LoadScene(const char* name);
	void Execute(float dt);
	void SetScenesGroupsState(const char* group, int state);
	void UnloadScene(const char* name);
	void UnloadAll();
};