
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

SceneObject* Scene::AddObject(const char* name, bool is_asset)
{
	SceneObject* obj = is_asset ? ClassFactorySceneAsset::Create(name) : ClassFactorySceneObject::Create(name);

	if (obj)
	{
		if (!is_asset)
		{
			obj->scriptClassName = is_asset ? ClassFactorySceneAsset::Find(name)->GetShortName() : ClassFactorySceneObject::Find(name)->GetShortName();
		}

		obj->owner = this;
		obj->className = name;
		obj->Init();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->SetDefValuesPrepare();

		if (is_asset)
		{
			assets.push_back(obj);
		}
		else
		{
			objects.push_back(obj);
		}
	}

	return obj;
}

SceneObject* Scene::FindByName(const char* name, std::vector<SceneObject*>& objects)
{
	for (auto obj : objects)
	{
		if (StringUtils::IsEqual(obj->GetName(), name))
		{
			return (SceneAsset*)obj;
		}
	}

	return nullptr;
}

SceneObject* Scene::FindByName(const char* name, bool is_asset)
{
	if (is_asset)
	{
		return FindByName(name, assets);
	}

	return FindByName(name, objects);
}

SceneObject* Scene::FindByUID(uint32_t uid, uint32_t child_uid, std::vector<SceneObject*>& objects)
{
	for (auto obj : objects)
	{
		if (obj->GetUID() == uid)
		{
			return (child_uid == 0) ? obj : obj->GetChild(child_uid);
		}
	}

	return nullptr;
}

SceneObject* Scene::FindByUID(uint32_t uid, uint32_t child_uid, bool is_asset)
{
	if (is_asset)
	{
		return FindByUID(uid, child_uid, assets);
	}

	return FindByUID(uid, child_uid, objects);
}

SceneObject* Scene::GetObj(int index, bool is_asset)
{
	return is_asset ? assets[index] : objects[index];
}

int Scene::GetObjectsCount(bool is_asset)
{
	return is_asset ? (int)assets.size() : (int)objects.size();
}

void Scene::DeleteObject(SceneObject* obj, bool is_asset)
{
	if (is_asset)
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
	else
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
}

void Scene::DeleteObjects(std::vector<SceneObject*>& objects)
{
	for (auto& obj : objects)
	{
		obj->Release();
	}

	objects.clear();
}

int Scene::GetObjectIndex(SceneObject* object, bool is_asset)
{
	int index = 0;

	if (is_asset)
	{
		for (auto& asset : assets)
		{
			if (object == asset)
			{
				return index;
			}

			index++;
		}
	}
	else
	{
		for (auto& obj : objects)
		{
			if (obj == object)
			{
				return index;
			}

			index++;
		}
	}

	return -1;
}

void Scene::Clear()
{
	DeleteObjects(objects);
	DeleteObjects(assets);
}

void Scene::Load(JSONReader& reader, std::vector<SceneObject*>& objects, const char* block, bool is_asset)
{
	while (reader.EnterBlock(block))
	{
		char type[512];
		reader.Read("type", type, 512);

		SceneObject* obj = nullptr;

		obj = AddObject(type, is_asset);

		if (obj)
		{
			char name[512];
			reader.Read("name", name, 512);
			reader.Read("uid", obj->uid);

			if (obj->uid == 0)
			{
				GenerateUID(obj, is_asset);
			}

			obj->SetName(name);

			if (obj->Is3DObject())
			{
				reader.Read("transform", obj->Trans());
			}

			if (obj->UsingCamera2DPos())
			{
				reader.Read("ed_camera_2d", obj->Camera2DPos());
			}

			obj->Load(reader);
		}

		reader.LeaveBlock();
	}

	for (auto asset : assets)
	{
		asset->ApplyProperties();
	}

	for (auto object : objects)
	{
		object->ApplyProperties();
	}
}

void Scene::Load(const char* name)
{
	JSONReader reader;
	
	if (reader.Parse(name))
	{
		reader.Read("camera_pos", camera_pos);
		Load(reader, assets, "SceneAsset", true);
		Load(reader, objects, "SceneObject", false);
	}
}

void Scene::Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block)
{
	writer.StartArray(block);

	for (int i = 0; i < objects.size(); i++)
	{
		writer.StartBlock(NULL);

		SceneObject* obj = objects[i];

		writer.Write("type", obj->GetClassName());
		writer.Write("name", obj->GetName());
		writer.Write("uid", obj->GetUID());

		if (obj->Is3DObject())
		{
			writer.Write("transform", obj->Trans());
		}
		
		if (obj->UsingCamera2DPos())
		{
			writer.Write("ed_camera_2d", obj->Camera2DPos());
		}

		obj->Save(writer);

		writer.FinishBlock();
	}

	writer.FinishArray();
}

void Scene::Save(const char* name)
{
	JSONWriter writer;

	if (writer.Start(name))
	{
		writer.Write("camera_pos", camera_pos);
		Save(writer, assets, "SceneAsset");
		Save(writer, objects, "SceneObject");
	}
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

	script = scripts.CreateContext();
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

	RELEASE(script)
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

void Scene::GenerateUID(SceneObject* obj, bool is_asset)
{
	uint32_t uid = 0;

	while (!uid)
	{
		float koef = rnd() * 0.99f;
		uid = (uint32_t)(koef * uint32_t(-1));
		uid = FindByUID(uid, 0, is_asset) ? 0 : uid;
	}

	obj->uid = uid;
}

void Scene::GenerateChildUID(SceneObject* obj)
{
	bool need_add = true;

	for (auto child : pool_childs)
	{
		if (child == obj)
		{
			need_add = false;
		}
	}

	if (need_add)
	{
		pool_childs.push_back(obj);
	}

	uint32_t uid = 0;

	while (!uid)
	{
		float koef = rnd() * 0.99f;
		uid = (uint32_t)(koef * uint32_t(-1));
		uid = FindByUID(uid, 0, pool_childs) ? 0 : uid;
	}

	obj->uid = uid;
}

SceneObject* Scene::FindInGroup(const char* group_name, const char* name)
{
	Scene::Group& group = GetGroup(group_name);

	for (auto object : group.objects)
	{
		if (StringUtils::IsEqual(object->GetName(), name))
		{
			return object;
		}
	}

	return nullptr;
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
	for (auto& group : groups)
	{
		DelFromGroup(group.second, obj);
	}
}

void Scene::Release()
{
	Clear();

	delete taskPool;
	render.DelTaskPool(renderTaskPool);

	delete this;
}