
#pragma once

#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>

#include "Services/Physics/Physics.h"

#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

class SceneObject;
class SceneAsset;

class Scene
{
	friend class SceneObject;

public:

	struct Group
	{
		std::vector<SceneObject*> objects;
	};

private:
	std::vector<SceneObject*> objects;
	std::vector<SceneObject*> assets;
	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderTaskPool;
	bool playing = false;
	Group emptyGroup;
	std::map<std::string, Group> groups;

	PhysScene* pscene = nullptr;;
	void DelFromGroup(Group& group, SceneObject* obj);

	void Load(JSONReader& reader, std::vector<SceneObject*>& objects, const char* block, bool is_asset);
	void Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block);

	void DeleteObjects(std::vector<SceneObject*>& objects);

public:

	Scene();
	virtual ~Scene();

	void Init();

	SceneObject* AddObject(const char* name, bool is_asset);
	SceneObject* Find(const char* name, bool is_asset);
	SceneObject* GetObj(int index, bool is_asset);
	int          GetObjectIndex(SceneObject* obj, bool is_asset);
	int          GetObjectsCount(bool is_asset);
	void         DeleteObject(SceneObject* obj, bool is_asset);

	void Clear();

	void Load(const char* name);
	void Save(const char* name);
	void Execute(float dt);
	void Play();
	void Stop();
	bool Playing();

	virtual void EnableTasks(bool enable);

	SceneObject* FindInGroup(const char* group_name, const char* name);
	Group& GetGroup(const char* name);
	void AddToGroup(SceneObject* obj, const char* name);
	void DelFromGroup(SceneObject* obj, const char* name);
	void DelFromAllGroups(SceneObject* obj);

	void Release();
};
