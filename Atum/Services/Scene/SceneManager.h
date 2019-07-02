
#pragma once

#include "Scene.h"
#include "Services/Physic/Physics.h"

/**
\ingroup gr_code_services_scene
*/

/**
\brief SceneManger

This manager handles loading and unloading scenes according data from a project.
Scenes in a project have different pathes but to load/unload scene a file name without
extenstion is needed.

*/

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

	vector<SceneHolder*> scenes_to_load;
	vector<SceneHolder*> scenes_to_delete;

	map<std::string, SceneHolder*> scenes_search;
	

	void LoadScene(SceneHolder* holder);
	void UnloadScene(SceneHolder* holder);

public:

#ifndef DOXYGEN_SKIP
	void Init();
	void LoadProject(const char* project_name);
	inline PhysScene* PScene() { return pscene; }
	void Execute(float dt);
	void UnloadAll();
#endif

	/** 
	\brief Load a scene

	\param[in] name Name of a scene (filename without extension)
	*/
	void LoadScene(const char* name);

	/**
	\brief Set state for scene objects from scene groups in all loaded scenes

	\param[in] group NAme of a group
	\param[in] state State which will be set for secene objects
	*/
	void SetScenesGroupsState(const char* group, int state);

	/**
	\brief Unload a scene

	\param[in] name Name of a scene (filename without extension)
	*/
	void UnloadScene(const char* name);
};