
#pragma once

#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>

#include "Services/Script/Scripts.h"

#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

class SceneObject;
class SceneAsset;
class SceneObjectComp;

/**
\ingroup gr_code_services_scene
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

class Scene
{
	friend class SceneObject;
	friend class SceneManager;

#ifdef EDITOR
	friend class Project;
#endif

public:

	struct Group
	{
		/** \brief Name of a group */
		string name;

		/** \brief Array of scene object */
		std::vector<SceneObject*> objects;
	};

#ifndef DOXYGEN_SKIP
	Vector2 camera3d_angles = 0.0f;
	Vector  camera3d_pos = 0.0f;
	Vector2 camera2d_pos = 0.0f;
	int move_mode = 0;
	int gizmo2d_align_x = 0;
	int gizmo2d_align_y = 0;

#ifdef EDITOR
	bool load_asset_inst = false;
	char project_scene_path[512];
#endif

#endif

private:
	std::vector<SceneObject*> objects;
	std::vector<SceneObject*> assets;
	std::vector<SceneObject*> pool_childs;
	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
	bool playing = false;

	std::map<std::string, Group> groups;

	ScriptContext* script = nullptr;

	void DelFromGroup(Group& group, SceneObject* obj, Scene* new_scene = nullptr);

	void Load(JSONReader& reader, std::vector<SceneObject*>& objects, const char* block, bool is_asset);
	void Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block);

	void DeleteObjects(std::vector<SceneObject*>& objects);
	SceneObject* FindByUID(uint32_t uid, uint32_t parent_uid, std::vector<SceneObject*>& objects);

	uint16_t uid = 0;

	char scene_path[512];
	char scene_name[512];

	std::vector<Scene*> inc_scenes;
public:

#ifndef DOXYGEN_SKIP

	Scene() = default;
	virtual ~Scene() = default;

	void Init();

	void AddObject(SceneObject* obj, bool is_asset);
	SceneObject* GetObj(int index, bool is_asset);
	int GetObjectIndex(SceneObject* obj, bool is_asset);
	int GetObjectsCount(bool is_asset);
	
	const char* GetPath();
	const char* GetName();

	void Clear();
	void Load(const char* name);
	void Save(const char* name);
	void Execute(float dt);

	bool Play();

	void EnableTasks(bool enable);
#endif

	/**
	\brief Create a scene object

	\param[in] name Name of a type
	\param[in] is_asset Is scene object is an asset

	\return Pointer to a scene object
	*/
	SceneObject* CreateObject(const char* name, bool is_asset);

	/**
	\brief Find a scene object by UID

	\param[in] uid UID of a scene object
	\param[in] child_uid UID of a child scene object (0 if neede only parent)
	\param[in] is_asset Is scene object is an asset

	\return Pointer to a scene object
	*/
	SceneObject* FindByUID(uint32_t uid, uint32_t child_uid, bool is_asset);

	/**
	\brief Delete a scene object from a scene

	\param[in] obj Pointer to a scene object
	\param[in] is_asset Is scene object is an asset
	\param[in] release_obj Should Release be called during deletion
	*/
	void DeleteObject(SceneObject* obj, bool is_asset, bool release_obj);

	/**
	\brief Find a scene object in group by name

	\param[in] group_name Name of a group
	\param[in] name Name of a scene object

	\return Pointer to a scene object
	*/
	SceneObject* FindInGroup(const char* group_name, const char* name);

	/**
	\brief Get Find a scene object in group by name

	\param[out] groups Array with all scene groups which are including included scenes
	\param[in] name Name of a group

	*/
	void GetGroup(vector<Group*>& groups, const char* name);

	/**
	\brief Adding a scene object to a group

	\param[in] obj Pointer to a scene object
	\param[in] name Name of a group
	*/
	void AddToGroup(SceneObject* obj, const char* name);

	/**
	\brief Deleting a scene object from a group

	\param[in] obj Pointer to a scene object
	\param[in] name Name of a group
	*/
	void DelFromGroup(SceneObject* obj, const char* name);

	/**
	\brief Deleting a scene object from all groups

	\param[in] obj Pointer to a scene object
	\param[in] new_scene Moving a scene object to all groups in new scene
	*/
	void DelFromAllGroups(SceneObject* obj, Scene* new_scene = nullptr);

	/**
	\brief Checking if scene is playing

	\return Returns true if a scene is playing. Otherwise false will be returned
	*/
	bool Playing();

#ifndef DOXYGEN_SKIP

#ifdef EDITOR
	bool DependFromScene(Scene* scene);
#endif

	void GenerateUID(SceneObject* obj, bool is_asset);
	void GenerateChildUID(SceneObject* obj);

	void Release();
#endif
};
