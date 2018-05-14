
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

	void Load(JSONReader& reader, std::vector<SceneObject*>& objects, const char* block);
	void Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block);

	void DeleteObjects(std::vector<SceneObject*>& objects);

public:

	Scene();
	virtual ~Scene();

	void Init();

	SceneObject* AddObject(const char* name);
	SceneObject* Find(const char* name);
	SceneObject* GetObj(int index);
	int  GetObjectsCount();
	void DeleteObject(SceneObject* obj);

	SceneAsset* AddAsset(const char* name);
	SceneAsset* FindAsset(const char* name);
	SceneAsset* GetAsset(int index);
	int  GetAssetsCount();
	void DeleteAsset(SceneAsset* obj);

	void DeleteAllObjects();

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
