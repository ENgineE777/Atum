
#include "Scene.h"
#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

Scene::Scene()
{

}

Scene::~Scene()
{

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