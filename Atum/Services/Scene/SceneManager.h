
#pragma once

#include "Scene.h"
#include "Services/Physic/Physics.h"

/**
\ingroup gr_code_services_scene
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

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

	vector<SceneHolder*> scenes_to_delete;

	map<std::string, SceneHolder*> scenes_search;

	void LoadScene(SceneHolder* holder);
	void UnloadScene(SceneHolder* holder);

public:

	void Init();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void LoadProject(const char* project_name);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	inline PhysScene* PScene() { return pscene; }

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void LoadScene(const char* name);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void Execute(float dt);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void SetScenesGroupsState(const char* group, int state);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void UnloadScene(const char* name);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void UnloadAll();
};