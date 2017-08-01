
#include "Scene.h"
#include "SceneObject.h"
#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

Scene::Scene()
{
	taskPool = NULL;
	renderTaskPool = NULL;
	playing = false;
}

Scene::~Scene()
{

}

void Scene::Init()
{
	taskPool = taskExecutor.CreateSingleTaskPool();
	renderTaskPool = render.AddTaskPool();
}

SceneObject* Scene::AddObject(const char* name)
{
	SceneObject* obj = ClassFactorySceneObject::Create(name);

	if (obj)
	{
		obj->owner = this;
		obj->className = name;
		obj->Init();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->SetDefValuesPrepare();

		objects.push_back(obj);
	}

	return obj;
}

SceneObject* Scene::Find(const char* name)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (StringUtils::IsEqual(objects[i]->GetName(), name))
		{
			return objects[i];
		}
	}

	return NULL;
}

SceneObject* Scene::GetObj(int index)
{
	return objects[index];
}

int Scene::GetObjectsCount()
{
	return objects.size();
}

void Scene::DeleteObject(SceneObject* obj)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == obj)
		{
			objects.erase(objects.begin() + i);
			obj->Release();
			break;
		}
	}
}

void Scene::DeleteAllObjects()
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Release();
	}

	objects.clear();
}

void Scene::Load(const char* name)
{
	JSONReader* reader = new JSONReader();
	
	if (reader->Parse(name))
	{
		while (reader->EnterBlock("SceneObject"))
		{
			char type[512];
			reader->Read("type", type, 512);

			SceneObject* obj = AddObject(type);

			if (obj)
			{
				char name[512];
				reader->Read("name", name, 512);

				obj->SetName(name);
				reader->Read("transform", obj->Trans());

				obj->GetMetaData()->Prepare(obj);
				obj->GetMetaData()->Load(reader);
				obj->ApplyProperties();
			}

			reader->LeaveBlock();
		}
	}

	reader->Release();
}

void Scene::Save(const char* name)
{
	JSONWriter* writer = new JSONWriter();

	if (writer->Start(name))
	{
		writer->StartArray("SceneObject");

		for (int i = 0; i < objects.size(); i++)
		{
			writer->StartBlock(NULL);

			SceneObject* obj = objects[i];

			writer->Write("type", obj->GetClassName());
			writer->Write("name", obj->GetName());
			writer->Write("transform", obj->Trans());

			obj->GetMetaData()->Prepare(obj);
			obj->GetMetaData()->Save(writer);

			writer->FinishBlock();
		}

		writer->FinishArray();
	}

	writer->Release();
}

void Scene::Execute(float dt)
{
	taskPool->Execute(dt);
}

void Scene::Play()
{
	playing = true;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Play();
	}
}

void Scene::Stop()
{
	playing = false;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Stop();
	}
}

bool Scene::Playing()
{
	return playing;
}

Scene::Group& Scene::GetGroup(const char* name)
{
	if (groups.find(name) == groups.end())
	{
		return emptyGroup;
	}

	return groups[name];
}

void Scene::AddToGroup(SceneObject* obj, const char* name)
{
	Group& group = groups[name];

	group.objects.push_back(obj);
}

void Scene::DelFromGroup(Group& group, SceneObject* obj)
{
	for (int i = 0; i < group.objects.size(); i++)
	{
		if (group.objects[i] == obj)
		{
			group.objects.erase(group.objects.begin() + i);
			break;
		}
	}
}

void Scene::DelFromGroup(SceneObject* obj, const char* name)
{
	DelFromGroup(groups[name], obj);
}

void Scene::DelFromAllGroups(SceneObject* obj)
{
	for (std::map<std::string, Group>::iterator it = groups.begin(); it != groups.end(); it++)
	{
		DelFromGroup(it->second, obj);
	}
}