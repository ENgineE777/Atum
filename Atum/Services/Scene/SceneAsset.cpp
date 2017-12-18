
#include "SceneAsset.h"

CLASSFACTORYDECL(SceneAsset)

SceneAsset::SceneAsset() : SceneObject()
{
	taskPool = taskExecutor.CreateSingleTaskPool();
	renderTaskPool = render.AddTaskPool();

	taskPool->SetActive(false);
	renderTaskPool->SetActive(false);
}

SceneAsset::~SceneAsset()
{

}

TaskExecutor::SingleTaskPool* SceneAsset::Tasks()
{
	return taskPool;
}

TaskExecutor::SingleTaskPool* SceneAsset::RenderTasks()
{
	return renderTaskPool;
}

void SceneAsset::EnableTasks(bool enable)
{
	taskPool->SetActive(enable);
	renderTaskPool->SetActive(enable);
}