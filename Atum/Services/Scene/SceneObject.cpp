
#include "SceneObject.h"

SceneObject::SceneObject()
{
	owner = nullptr;
}

SceneObject::~SceneObject()
{

}

bool SceneObject::EnableTasks(bool enable)
{
	if (taskPool)
	{
		taskPool->SetActive(enable);;
	}

	if (renderTaskPool)
	{
		renderTaskPool->SetActive(enable);
	}

	return taskPool || renderTaskPool;
}

void SceneObject::ApplyProperties()
{

}

const char* SceneObject::GetName()
{
	return name.c_str();
}

void SceneObject::SetName(const char* set_name)
{
	name = set_name;
}

Matrix& SceneObject::Trans()
{
	return transform;
}

const char* SceneObject::GetClassName()
{
	return className.c_str();
}

void SceneObject::Load(JSONReader& reader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(reader);
}

void SceneObject::Save(JSONWriter& writer)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(writer);
}

TaskExecutor::SingleTaskPool* SceneObject::Tasks(bool editor)
{
	if (editor)
	{
		if (!taskPool)
		{
			taskPool = taskExecutor.CreateSingleTaskPool();
			taskPool->SetActive(false);
		}

		return taskPool;
	}

	return owner->taskPool;
}

TaskExecutor::SingleTaskPool* SceneObject::RenderTasks(bool editor)
{
	if (editor)
	{
		if (!renderTaskPool)
		{
			renderTaskPool = renderTaskPool = render.AddTaskPool();
			renderTaskPool->SetActive(false);
		}

		return renderTaskPool;
	}

	return owner->renderTaskPool;
}

void SceneObject::Play()
{
	initTransform = transform;
}

void SceneObject::Stop()
{
	transform = initTransform;
}

bool SceneObject::Playing()
{
	return owner->playing;
}

ScriptContext* SceneObject::Scipt()
{
	return owner->script;
}

PhysScene* SceneObject::PScene()
{
	return owner->pscene;
}

void SceneObject::Release()
{
	owner->taskPool->DelAllTasks(this);

	if (taskPool)
	{
		taskPool->DelAllTasks(this);
	}

	owner->renderTaskPool->DelAllTasks(this);

	if (renderTaskPool)
	{
		renderTaskPool->DelAllTasks(this);
	}

	owner->DelFromAllGroups(this);

	delete this;
}

#ifdef EDITOR
void SceneObject::CheckProperties()
{
	if (GetMetaData()->IsValueWasChanged())
	{
		ApplyProperties();
	}
}

void SceneObject::Copy(SceneObject* src)
{
	Trans() = src->Trans();
	src->GetMetaData()->Copy(src);
	ApplyProperties();
}

void SceneObject::SetEditMode(bool ed)
{
	edited = ed;
}

void SceneObject::OnMouseMove(Vector2 delta_ms)
{

}

void SceneObject::OnLeftMouseDown(Vector2 ms)
{

}

void SceneObject::OnLeftMouseUp()
{

}

void SceneObject::OnRightMouseDown(Vector2 ms)
{

}

void SceneObject::OnRightMouseUp()
{

}
#endif