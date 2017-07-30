
#pragma once

#include "Services/Render/Device.h"
#include "Services/Render/Program.h"
#include <vector>
#include <map>

class TaskExecutor
{
public:

	class TaskPool
	{
	public:
		virtual void Execute(float dt) = 0;
	};

	class GroupTaskPool;

	class SingleTaskPool : public TaskPool
	{
	public:

		friend class GroupTaskPool;

		struct Task : DelegateObject
		{
			float freq;
			float time;
		};

		struct TaskList
		{
			int  level;
			std::vector<Task> list;
		};

	private:

		std::vector<TaskList*> lists;

		bool active;
		int changeMark;

		TaskList* FindTaskList(int level);
		static void ExecuteList(TaskList* list, float dt);

	public:

		SingleTaskPool();

		void SetActive(bool set);
		virtual void Execute(float dt);

		void AddTask(int level, Object* entity, Object::Delegate call, float freq = -1.0f);
		void DelTask(int level, Object* entity);
		void DelAllTasks(Object* entity);
	};

	class GroupTaskPool : public TaskPool
	{
		friend class SingleTaskPool;

		struct TaskList
		{
			SingleTaskPool* pool;
			SingleTaskPool::TaskList* list;
		};

		struct GroupList
		{
			int level;
			std::vector<TaskList> taskLists;
		};

		std::vector<GroupList> groupLists;
		std::vector<SingleTaskPool*> taskPools;
		std::vector<int> changeMarks;
		std::vector<int> filter;

		void FillList();
		void Execute(GroupList& groupList, float dt);

	public:

		void AddFilter(int level);

		SingleTaskPool* AddTaskPool();
		void DelTaskPool(SingleTaskPool* pool);
		virtual void Execute(float dt);
		void ExecutePool(int level, float dt);
	};

	SingleTaskPool* CreateSingleTaskPool();
	GroupTaskPool* CreateGroupTaskPool();

	void SetTaskPoolExecutionLevel(TaskPool* taskPool, int level);
	void Execute(float dt);

private:

	struct Pool
	{
		int level;
		TaskPool* taskPool;
	};

	std::vector<Pool> pools;
};

extern TaskExecutor taskExecutor;