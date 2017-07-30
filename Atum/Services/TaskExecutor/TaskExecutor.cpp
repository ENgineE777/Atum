
#include "TaskExecutor.h"

TaskExecutor taskExecutor;

TaskExecutor::SingleTaskPool::SingleTaskPool()
{
	active = true;
	changeMark = 0;
}

void TaskExecutor::SingleTaskPool::SetActive(bool set)
{
	active = set;
}

void TaskExecutor::SingleTaskPool::ExecuteList(TaskList* list, float dt)
{
	for (int j = 0; j<list->list.size(); j++)
	{
		(list->list[j].entity->*list->list[j].call)(dt);
	}
}

void TaskExecutor::SingleTaskPool::Execute(float dt)
{
	for (int i = 0; i<lists.size(); i++)
	{
		ExecuteList(lists[i], dt);
	}
}

TaskExecutor::SingleTaskPool::TaskList* TaskExecutor::SingleTaskPool::FindTaskList(int level)
{
	for (int i = 0; i < lists.size(); i++)
	{
		if (lists[i]->level == level)
		{
			return lists[i];
		}
	}

	return NULL;
}

void TaskExecutor::SingleTaskPool::AddTask(int level, Object* entity, Object::Delegate call, float freq)
{
	TaskList* list = FindTaskList(level);

	if (!list)
	{
		lists.push_back(new TaskList());
		list = lists[lists.size() - 1];
		list->level = level;

		int index = lists.size() - 1;

		while (index>0 && list[index - 1].level > list->level)
		{
			TaskList* tmp = lists[index];
			lists[index] = lists[index - 1];
			lists[index - 1] = tmp;

			index--;
		}
	}

	list->list.push_back(Task());
	Task* delegate = &list->list[list->list.size() - 1];

	delegate->entity = entity;
	delegate->call = call;
	delegate->freq = freq;
	delegate->time = -1.0f;

	changeMark++;

	if (changeMark > 1000000)
	{
		changeMark = 0;
	}
}

void TaskExecutor::SingleTaskPool::DelTask(int level, Object* entity)
{
	TaskList* list = FindTaskList(level);

	if (list)
	{
		for (int i = 0; i<list->list.size(); i++)
		{
			if (entity == list->list[i].entity)
			{
				list->list.erase(list->list.begin() + i);
				break;
			}
		}

		changeMark++;
		if (changeMark > 1000000)
		{
			changeMark = 0;
		}
	}
}

void TaskExecutor::SingleTaskPool::DelAllTasks(Object* entity)
{
	for (int i = 0; i < lists.size(); i++)
	{
		DelTask(lists[i]->level, entity);
	}
}

void TaskExecutor::GroupTaskPool::FillList()
{
	groupLists.clear();

	for (int i = 0; i < taskPools.size(); i++)
	{
		SingleTaskPool* taskPool = taskPools[i];

		for (int j = 0; j < taskPool->lists.size(); j++)
		{
			SingleTaskPool::TaskList* taskList = taskPool->lists[j];

			GroupList* groupList = NULL;

			for (int l = 0; l < groupLists.size(); l++)
			{
				if (groupLists[l].level == taskList->level)
				{
					groupList = &groupLists[l];

					break;
				}
			}

			if (!groupList)
			{
				groupLists.push_back(GroupList());
				groupList = &groupLists[groupLists.size() - 1];
				groupList->level = taskList->level;

				int index = groupLists.size() - 1;

				while (index>0 && groupLists[index - 1].level > groupList->level)
				{
					GroupList tmp = groupLists[index];
					groupLists[index] = groupLists[index - 1];
					groupLists[index - 1] = tmp;

					groupList = &groupLists[index - 1];

					index--;
				}
			}

			groupList->taskLists.push_back(TaskList());

			TaskList& list = groupList->taskLists[groupList->taskLists.size() - 1];
			list.pool = taskPool;
			list.list = taskList;
		}
	}
}

void TaskExecutor::GroupTaskPool::AddFilter(int level)
{
	for (int i = 0; i < filter.size(); i++)
	{
		if (filter[i] == level)
		{
			return;
		}
	}

	filter.push_back(level);

	int index = filter.size() - 1;

	while (index>0 && filter[index - 1] > level)
	{
		int tmp = filter[index];
		filter[index] = filter[index - 1];
		filter[index - 1] = tmp;

		index--;
	}
}

TaskExecutor::SingleTaskPool* TaskExecutor::GroupTaskPool::AddTaskPool()
{
	SingleTaskPool* pool = new SingleTaskPool();

	taskPools.push_back(pool);

	return pool;
}

void TaskExecutor::GroupTaskPool::DelTaskPool(SingleTaskPool* pool)
{
	for (int i = 0; i < taskPools.size(); i++)
	{
		if (taskPools[i] == pool)
		{
			taskPools.erase(taskPools.begin() + i);
			return;
		}
	}
}

void TaskExecutor::GroupTaskPool::Execute(GroupList& groupList, float dt)
{
	for (int j = 0; j < groupList.taskLists.size(); j++)
	{
		TaskList& taskList = groupList.taskLists[j];

		if (taskList.pool->active)
		{
			SingleTaskPool::ExecuteList(taskList.list, dt);
		}
	}
}

void TaskExecutor::GroupTaskPool::Execute(float dt)
{
	FillList();

	int filter_index = (filter.size() > 0) ? 0 : -1;

	for (int i = 0; i < groupLists.size(); i++)
	{
		GroupList& groupList = groupLists[i];

		if (filter_index != -1)
		{
			while (filter[filter_index] < groupList.level)
			{
				filter_index++;

				if (filter_index == filter.size())
				{
					return;
				}
			}
		}

		if (filter_index != -1 && filter[filter_index] != groupList.level)
		{
			continue;
		}

		Execute(groupList, dt);
	}
}

void TaskExecutor::GroupTaskPool::ExecutePool(int level, float dt)
{
	for (int i = 0; i < groupLists.size(); i++)
	{
		GroupList& groupList = groupLists[i];

		if (groupList.level != level)
		{
			continue;
		}

		Execute(groupList, dt);
	}
}

TaskExecutor::SingleTaskPool* TaskExecutor::CreateSingleTaskPool()
{
	return new SingleTaskPool();
}

TaskExecutor::GroupTaskPool* TaskExecutor::CreateGroupTaskPool()
{
	return new GroupTaskPool();
}

void TaskExecutor::SetTaskPoolExecutionLevel(TaskPool* taskPool, int level)
{
	pools.push_back(Pool());

	Pool& pool = pools[pools.size() - 1];
	pool.level = level;
	pool.taskPool = taskPool;

	int index = pools.size() - 1;

	while (index>0 && pools[index - 1].level > level)
	{
		Pool tmp = pools[index];
		pools[index] = pools[index - 1];
		pools[index - 1] = tmp;

		index--;
	}
}

void TaskExecutor::Execute(float dt)
{
	for (int i = 0; i < pools.size(); i++)
	{
		pools[i].taskPool->Execute(dt);
	}
}