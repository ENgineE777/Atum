
#pragma once

#include "SceneObject.h"

class SceneAsset : public SceneObject
{
	friend class Scene;
	friend class Scene;

	TaskExecutor::SingleTaskPool* taskPool;
	TaskExecutor::SingleTaskPool* renderTaskPool;

public:

	SceneAsset();
	virtual ~SceneAsset();

	virtual TaskExecutor::SingleTaskPool* Tasks();
	virtual TaskExecutor::SingleTaskPool* RenderTasks();
	virtual void EnableTasks(bool enable);
};

CLASSFACTORYDEF(SceneAsset)
