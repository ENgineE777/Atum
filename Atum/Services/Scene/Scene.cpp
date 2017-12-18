
#include "Scene.h"
#include "SceneObject.h"
#include "SceneAsset.h"

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
	return (int)objects.size();
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

SceneAsset* Scene::AddAsset(const char* name)
{
	SceneAsset* obj = ClassFactorySceneAsset::Create(name);

	if (obj)
	{
		obj->owner = this;
		obj->className = name;
		obj->Init();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->SetDefValuesPrepare();

		assets.push_back(obj);
	}

	return obj;
}

SceneAsset* Scene::FindAsset(const char* name)
{
	for (int i = 0; i < assets.size(); i++)
	{
		if (StringUtils::IsEqual(assets[i]->GetName(), name))
		{
			return (SceneAsset*)assets[i];
		}
	}

	return NULL;
}

SceneAsset* Scene::GetAsset(int index)
{
	return (SceneAsset*)assets[index];
}

int Scene::GetAssetsCount()
{
	return (int)assets.size();
}

void Scene::DeleteAsset(SceneAsset* obj)
{
	for (int i = 0; i < assets.size(); i++)
	{
		if ((SceneAsset*)assets[i] == obj)
		{
			assets.erase(assets.begin() + i);
			obj->Release();
			break;
		}
	}
}

void Scene::DeleteObjects(std::vector<SceneObject*>& objects)
{
	for (auto& obj : objects)
	{
		obj->Release();
	}

	objects.clear();
}

void Scene::DeleteAllObjects()
{
	DeleteObjects(objects);
	DeleteObjects(assets);
}

void Scene::Load(JSONReader* reader, std::vector<SceneObject*>& objects, const char* block)
{
	while (reader->EnterBlock(block))
	{
		char type[512];
		reader->Read("type", type, 512);

		SceneObject* obj = nullptr;

		if (StringUtils::IsEqual(block, "SceneAsset"))
		{
			obj = AddAsset(type);
		}
		else
		{
			obj = AddObject(type);
		}

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

void Scene::Load(const char* name)
{
	JSONReader* reader = new JSONReader();
	
	if (reader->Parse(name))
	{
		Load(reader, assets, "SceneAsset");
		Load(reader, objects, "SceneObject");
	}

	reader->Release();
}

void Scene::Save(JSONWriter* writer, std::vector<SceneObject*>& objects, const char* block)
{
	writer->StartArray(block);

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

void Scene::Save(const char* name)
{
	JSONWriter* writer = new JSONWriter();

	if (writer->Start(name))
	{
		Save(writer, assets, "SceneAsset");
		Save(writer, objects, "SceneObject");
	}

	writer->Release();
}

void Scene::Execute(float dt)
{
	taskPool->Execute(dt);
}

void Scene::Play()
{
	if (playing)
	{
		return;
	}

	playing = true;

	pscene = physics.CreateScene();

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Play();
	}
}

void Scene::Stop()
{
	if (!playing)
	{
		return;
	}

	playing = false;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Stop();
	}

	physics.DestroyScene(pscene);
	pscene = nullptr;
}

bool Scene::Playing()
{
	return playing;
}

void Scene::EnableTasks(bool enable)
{
	taskPool->SetActive(enable);
	renderTaskPool->SetActive(enable);
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