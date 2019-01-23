
#pragma once

#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>

#include "Services/Script/Scripts.h"
#include "Services/Physic/Physics.h"

#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

class SceneObject;
class SceneAsset;
class SceneObjectComp;

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
		std::vector<SceneObject*> objects;
	};

	Vector2 camera3d_angles = 0.0f;
	Vector  camera3d_pos = 0.0f;
	Vector2 camera2d_pos = 0.0f;
	int move_mode = 0;
	int gizmo2d_align_x = 0;
	int gizmo2d_align_y = 0;
	PhysScene* pscene = nullptr;

private:
	std::vector<SceneObject*> objects;
	std::vector<SceneObject*> assets;
	std::vector<SceneObject*> pool_childs;
	TaskExecutor::SingleTaskPool* taskPool = nullptr;
	TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
	bool playing = false;
	Group emptyGroup;
	std::map<std::string, Group> groups;

	ScriptContext* script = nullptr;

	void DelFromGroup(Group& group, SceneObject* obj);

	void Load(JSONReader& reader, std::vector<SceneObject*>& objects, const char* block, bool is_asset);
	void Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block);

	void DeleteObjects(std::vector<SceneObject*>& objects);
	SceneObject* FindByName(const char* name, std::vector<SceneObject*>& objects);
	SceneObject* FindByUID(uint32_t uid, uint32_t parent_uid, std::vector<SceneObject*>& objects);

	char scene_path[512];
	char scene_name[512];

	std::vector<Scene*> inc_scenes;
public:

	Scene() = default;
	virtual ~Scene() = default;

	void Init();

	SceneObject* AddObject(const char* name, bool is_asset);
	SceneObject* FindByName(const char* name, bool is_asset);
	SceneObject* FindByUID(uint32_t uid, uint32_t child_uid, bool is_asset);
	SceneObject* GetObj(int index, bool is_asset);
	int          GetObjectIndex(SceneObject* obj, bool is_asset);
	int          GetObjectsCount(bool is_asset);
	void         DeleteObject(SceneObject* obj, bool is_asset);

	const char* GetScenePath();
	const char* GetSceneName();
	void Clear();

	void Load(const char* name);
	void Save(const char* name);
	void Execute(float dt);
	bool Play();
	void Stop();
	bool Playing();

	virtual void EnableTasks(bool enable);

	void GenerateUID(SceneObject* obj, bool is_asset);
	void GenerateChildUID(SceneObject* obj);
	SceneObject* FindInGroup(const char* group_name, const char* name);
	Group& GetGroup(const char* name);
	void AddToGroup(SceneObject* obj, const char* name);
	void DelFromGroup(SceneObject* obj, const char* name);
	void DelFromAllGroups(SceneObject* obj);

	void Release();
};
