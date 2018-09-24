
#include "Scene.h"
#include "SceneObject.h"
#include "SceneAsset.h"

#include "SceneAssets/Phys2DComp.h"
#include "SceneAssets/SpriteGraphInst.h"
#include "SceneAssets/SpriteInst.h"
#include "Services/Script/Libs/scriptarray.h"

int index_hack_show_message = 0;

void Scene::ContactListener::BeginContact(b2Contact* contact)
{
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	if (bodyUserDataA && bodyUserDataB)
	{
		Phys2DCompInst::BodyUserData* udataA = static_cast<Phys2DCompInst::BodyUserData*>(bodyUserDataA);
		Phys2DCompInst::BodyUserData* udataB = static_cast<Phys2DCompInst::BodyUserData*>(bodyUserDataB);

		if (udataA->object->OnContact(udataA->index, udataB->object, udataB->index))
		{
			contact->SetEnabled(false);
		}

		if (udataB->object->OnContact(udataB->index, udataA->object, udataA->index))
		{
			contact->SetEnabled(false);
		}
	}
}

void Scene::ContactListener::EndContact(b2Contact* contact)
{
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	if (bodyUserDataA && bodyUserDataB)
	{
		Phys2DCompInst::BodyUserData* udataA = static_cast<Phys2DCompInst::BodyUserData*>(bodyUserDataA);
		Phys2DCompInst::BodyUserData* udataB = static_cast<Phys2DCompInst::BodyUserData*>(bodyUserDataB);

		if (StringUtils::IsEqual(udataA->object->GetName(), "Hero_Graph") ||
			StringUtils::IsEqual(udataB->object->GetName(), "Hero_Graph"))
		{
			SpriteGraphInst* graph = dynamic_cast<SpriteGraphInst*>(udataA->object);
			SceneObject* object = nullptr;
			int index = udataB->index;

			if (!graph)
			{
				object = static_cast<SceneObject*>(udataA->object);
				index = udataA->index;

				graph = dynamic_cast<SpriteGraphInst*>(udataB->object);
			}
			else
			{
				object = static_cast<SceneObject*>(udataB->object);
			}

			if (graph && object)
			{
				if (StringUtils::IsEqual(object->GetName(), "TestTrigger"))
				{
					index_hack_show_message--;
				}
			}
		}
	}
}

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
	ClassFactorySceneObject* decl_objects;
	ClassFactorySceneAsset* decl_assets;

	if (!is_asset)
	{
		decl_objects = ClassFactorySceneObject::Find(name);
	}
	else
	{
		decl_assets = ClassFactorySceneAsset::Find(name);
	}

	SceneObject* obj = is_asset ? decl_assets->Create(name) : decl_objects->Create(name);

	if (obj)
	{
		if (!is_asset)
		{
			obj->script_class_name = is_asset ? decl_assets->GetShortName() : decl_objects->GetShortName();
		}

		obj->owner = this;
		obj->class_name = is_asset ? decl_assets->GetName() : decl_objects->GetName();
		obj->Init();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->SetDefValues();

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
			reader.Read("uid", obj->uid);

			if (obj->uid == 0)
			{
				GenerateUID(obj, is_asset);
			}

			if (obj->Is3DObject())
			{
				reader.Read("transform", obj->Trans());
			}

			if (obj->UsingCamera2DPos())
			{
				reader.Read("ed_camera_2d", obj->Camera2DPos());
			}

			obj->Load(reader);

			while (reader.EnterBlock("components"))
			{
				reader.Read("type", type, 512);

				SceneObjectComp* comp = obj->AddComponent(type);

				if (comp)
				{
					comp->Load(reader);
				}

				reader.LeaveBlock();
			}
		}

		reader.LeaveBlock();
	}

	for (auto obj : objects)
	{
		obj->ApplyProperties();

		for (auto comp : obj->components)
		{
			comp->ApplyProperties();
		}
	}
}

void Scene::Load(const char* name)
{
	JSONReader reader;
	
	if (reader.Parse(name))
	{
		reader.Read("camera_pos", camera_pos);
		reader.Read("move_mode", move_mode);
		reader.Read("gizmo2d_align_x", gizmo2d_align_x);
		reader.Read("gizmo2d_align_y", gizmo2d_align_y);
		Load(reader, assets, "SceneAsset", true);
		Load(reader, objects, "SceneObject", false);

		int index = 0;

		while (reader.EnterBlock("asset_inctances"))
		{
			uint32_t asset_uid = 0;
			uint32_t inst_uid = 0;

			reader.Read("asset_uid", asset_uid);
			reader.Read("inst_uid", inst_uid);

			SceneAsset* asset = (SceneAsset*)FindByUID(asset_uid, 0, true);
			SceneObject* asset_inst = FindByUID(inst_uid, 0, false);

			bool added = false;

			for (auto inst : asset->instances)
			{
				if (inst == asset_inst)
				{
					added = true;
					break;
				}
			}

			if (!added)
			{
				asset->instances.push_back(asset_inst);
			}

			reader.LeaveBlock();

			index++;
		}
	}
}

void Scene::Save(JSONWriter& writer, std::vector<SceneObject*>& objects, const char* block)
{
	writer.StartArray(block);

	for (auto& obj : objects)
	{
		writer.StartBlock(nullptr);

		writer.Write("type", obj->class_name);
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

		if (obj->components.size() > 0)
		{
			writer.StartArray("components");

			for (auto& comp : obj->components)
			{
				writer.StartBlock(nullptr);

				writer.Write("type", comp->class_name);
				comp->Save(writer);

				writer.FinishBlock();
			}

			writer.FinishArray();
		}

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
		writer.Write("move_mode", move_mode);
		writer.Write("gizmo2d_align_x", gizmo2d_align_x);
		writer.Write("gizmo2d_align_y", gizmo2d_align_y);

		Save(writer, assets, "SceneAsset");
		Save(writer, objects, "SceneObject");

		writer.StartArray("asset_inctances");

		for (auto obj : assets)
		{
			SceneAsset* asset = (SceneAsset*)obj;
			for (auto inst : asset->instances)
			{
				writer.StartBlock(nullptr);

				writer.Write("asset_uid", asset->GetUID());
				writer.Write("inst_uid", inst->GetUID());

				writer.FinishBlock();
			}
		}

		writer.FinishArray();
	}
}

void Scene::Execute(float dt)
{
	taskPool->Execute(dt);
}

bool Scene::Play()
{
	if (playing)
	{
		return false;
	}

	playing = true;

	script = scripts.CreateContext();
	pscene = physics.CreateScene();

	pscene2D = physics.CreateScene2D();
	pscene2D->SetContactListener(&contact_listiner);
	//pscene2D->SetContactFilter(&contact_filter);

	for (auto object : objects)
	{
		if (!object->Play())
		{
			return false;
		}

		for (auto comp : object->components)
		{
			comp->Play();
		}
	}

	return true;
}

void Scene::Stop()
{
	if (!playing)
	{
		return;
	}

	playing = false;

	for (auto object : objects)
	{
		object->Stop();

		for (auto comp : object->components)
		{
			comp->Stop();
		}
	}

	RELEASE(script)
	physics.DestroyScene(pscene);
	physics.DestroyScene2D(pscene2D);
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

#ifdef EDITOR
SceneObject* Scene::CheckSelection(Vector2 ms)
{
	vector<SceneObject*> tmp_under_selection;

	for (auto& obj : objects)
	{
		if (obj->CheckSelection(ms))
		{
			tmp_under_selection.push_back(obj);
		}
	}

	bool same_selection = true;

	if (tmp_under_selection.size() != under_selection.size())
	{
		same_selection = false;
	}
	else
	{
		for (int index = 0; index < under_selection.size(); index++)
		{
			if (under_selection[index] != tmp_under_selection[index])
			{
				same_selection = false;
				break;
			}
		}
	}

	if (same_selection)
	{
		under_selection_index++;

		if (under_selection_index >= under_selection.size())
		{
			under_selection_index = 0;
		}
	}
	else
	{
		under_selection_index = 0;
		under_selection = tmp_under_selection;
	}

	return under_selection.size() > 0 ? under_selection[under_selection_index] : nullptr;
}
#endif

void Scene::Release()
{
	Clear();

	delete taskPool;
	render.DelTaskPool(renderTaskPool);

	delete this;
}