
#include "SceneObject.h"

CLASSFACTORYDECL(SceneObject)

SceneObject::SceneObject()
{
	owner = NULL;
}

SceneObject::~SceneObject()
{

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

void SceneObject::Load(JSONReader* reader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(reader);
	ApplyProperties();
}

void SceneObject::Save(JSONWriter* writer)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(writer);
}

TaskExecutor::SingleTaskPool* SceneObject::Tasks()
{
	return owner->taskPool;
}

TaskExecutor::SingleTaskPool* SceneObject::RenderTasks()
{
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

PhysScene* SceneObject::PScene()
{
	return owner->pscene;
}

void SceneObject::Release()
{
	Tasks()->DelAllTasks(this);
	RenderTasks()->DelAllTasks(this);

	owner->DelFromAllGroups(this);

	delete this;
}

#ifdef EDITOR
void SceneObject::SetEditMode(bool ed)
{
	edited = ed;
}

void SceneObject::OnMouseMove(int dmx, int dmy)
{

}

void SceneObject::OnLeftMouseDown(int mx, int my)
{

}

void SceneObject::OnLeftMouseUp(int mx, int my)
{

}

void SceneObject::OnRightMouseDown(int mx, int my)
{

}

void SceneObject::OnRightMouseUp(int mx, int my)
{

}
#endif