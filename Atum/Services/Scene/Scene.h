
#pragma once

#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>

class SceneObject;

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
	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderTaskPool;
	bool playing;
	Group emptyGroup;
	std::map<std::string, Group> groups;

	void DelFromGroup(Group& group, SceneObject* obj);

public:

	Scene();
	virtual ~Scene();

	void Init();

	SceneObject* AddObject(const char* name);
	SceneObject* Find(const char* name);
	SceneObject* GetObj(int index);
	int  GetObjectsCount();
	void DeleteObject(SceneObject* obj);
	void DeleteAllObjects();
	void Load(const char* name);
	void Save(const char* name);
	void Execute(float dt);
	void Play();
	void Stop();
	bool Playing();

	Group& GetGroup(const char* name);
	void AddToGroup(SceneObject* obj, const char* name);
	void DelFromGroup(SceneObject* obj, const char* name);
	void DelFromAllGroups(SceneObject* obj);
};
