
#pragma once

#include "Support/Support.h"
#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"
#include <string>
#include "Services/TaskExecutor/TaskExecutor.h"
#include "Services/Render/Render.h"
#include "Services/Controls/Controls.h"
#include "Scene.h"

class SceneObject : public Object
{
	friend class Scene;

protected:
	Scene* owner;
	std::string  name;
	std::string  className;
	Matrix       transform;
	Matrix       initTransform;

public:

	bool test;

	SceneObject();
	virtual ~SceneObject();

	const char* GetName();
	void SetName(const char* name);
	Matrix& Trans();
	const char* GetClassName();

	virtual void Init() = 0;
	virtual void ApplyProperties();
	virtual MetaData* GetMetaData() = 0;
	TaskExecutor::SingleTaskPool* Tasks();
	TaskExecutor::SingleTaskPool* RenderTasks();
	virtual void Play();
	virtual void Stop();
	bool Playing();
	virtual void Release();
};

CLASSFACTORYDEF(SceneObject)
