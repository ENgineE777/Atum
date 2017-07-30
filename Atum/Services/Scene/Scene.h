
#pragma once

#include "SceneObject.h"
#include <vector>

class Scene
{
	std::vector<SceneObject*> objects;

public:

	Scene();
	virtual ~Scene();

	SceneObject* AddObject(const char* name);
	SceneObject* Find(const char* name);
	SceneObject* GetObj(int index);
	int  GetObjectsCount();
	void DeleteObject(SceneObject* obj);
	void DeleteAllObjects();
	void Load(const char* name);
	void Save(const char* name);
};
