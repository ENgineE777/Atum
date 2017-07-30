
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>

class Scene;

class SceneObject : public Object
{
	friend class Scene;

protected:
	Scene* owner;
	std::string  name;
	std::string  className;
	Matrix       transform;

public:

	bool test;

	SceneObject();
	virtual ~SceneObject();

	const char* GetName();
	void SetName(const char* name);
	Matrix& Trans();
	const char* GetClassName();

	virtual void Init() = 0;
	virtual MetaData* GetMetaData() = 0;
	virtual void Release();
};

CLASSFACTORYDEF(SceneObject)
