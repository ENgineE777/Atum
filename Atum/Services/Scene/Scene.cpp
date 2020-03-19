
#include "Scene.h"
#include "SceneObject.h"
#include "SceneAsset.h"

#include "SceneObjects/2D/Phys2DComp.h"
#include "SceneObjects/2D/SpriteGraphInst.h"
#include "SceneObjects/2D/SpriteInst.h"
#include "Services/Script/Libs/scriptarray.h"

void Scene::Init()
{
	scene_path[0] = 0;
	scene_name[0] = 0;

#ifdef EDITOR
	project_scene_path[0] = 0;
#endif

	taskPool = core.taskExecutor.CreateSingleTaskPool();
	renderTaskPool = core.render.AddTaskPool();
}

SceneObject* Scene::CreateObject(const char* name, bool is_asset)
{
	ClassFactorySceneObject* decl_objects;
	ClassFactorySceneAsset* decl_assets;

	if (!is_asset)
	{
		decl_objects = ClassFactorySceneObject::Find(name);

		if (!decl_objects)
		{
			return nullptr;
		}
	}
	else
	{
		decl_assets = ClassFactorySceneAsset::Find(name);

		if (!decl_assets)
		{
			return nullptr;
		}
	}

	SceneObject* obj = is_asset ? decl_assets->Create() : decl_objects->Create();

	if (obj)
	{
		if (!is_asset)
		{
			obj->script_class_name = is_asset ? decl_assets->GetShortName() : decl_objects->GetShortName();
		}

		obj->scene = this;
		obj->class_name = is_asset ? decl_assets->GetName() : decl_objects->GetName();
		obj->Init();

		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->SetDefValues();

		AddObject(obj, is_asset);
	}

	return obj;
}

void Scene::AddObject(SceneObject* obj, bool is_asset)
{
	if (is_asset)
	{
		assets.push_back(obj);
	}
	else
	{
		objects.push_back(obj);
	}
}

SceneObject* Scene::FindByUID(uint32_t uid, uint32_t child_uid, std::vector<SceneObject*>& objects)
{
	for (auto obj : objects)
	{
		if (obj && obj->GetUID() == uid)
		{
			return (child_uid == 0) ? obj : obj->GetChild(child_uid);
		}
	}

	return nullptr;
}

SceneObject* Scene::FindByUID(uint32_t uid, uint32_t child_uid, bool is_asset)
{
	SceneObject* res = nullptr;

	if (is_asset)
	{
		res = FindByUID(uid, child_uid, assets);
	}
	else
	{
		res = FindByUID(uid, child_uid, objects);
	}

	if (!res)
	{
		for (auto& incl : inc_scenes)
		{
			res = incl->FindByUID(uid, child_uid, is_asset);

			if (res)
			{
				break;
			}
		}
	}

	return res;
}

SceneObject* Scene::GetObj(int index, bool is_asset)
{
	return is_asset ? assets[index] : objects[index];
}

int Scene::GetObjectsCount(bool is_asset)
{
	return is_asset ? (int)assets.size() : (int)objects.size();
}

void Scene::DeleteObject(SceneObject* obj, bool is_asset, bool release_obj)
{
	if (is_asset)
	{
		for (int i = 0; i < assets.size(); i++)
		{
			if ((SceneAsset*)assets[i] == obj)
			{
				assets.erase(assets.begin() + i);
				if (release_obj) obj->Release();
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
				if (release_obj) obj->Release();
				break;
			}
		}
	}
}

void Scene::DeleteObjects(std::vector<SceneObject*>& objects)
{
	for (auto& obj : objects)
	{
		RELEASE(obj);
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

const char* Scene::GetPath()
{
	return scene_path;
}

const char* Scene::GetName()
{
	return scene_name;
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

		obj = CreateObject(type, is_asset);

		if (obj)
		{
			reader.Read("uid", obj->uid);

			if (obj->uid == 0)
			{
				GenerateUID(obj, is_asset);
			}

			if (obj->Is3DObject() && obj->Trans())
			{
				reader.Read("transform", obj->Trans());
			}

			if (obj->UsingOwnCamera())
			{
				if (obj->Is3DObject())
				{
					reader.Read("ed_camera_3d_pos", obj->camera3d_pos);
					reader.Read("ed_camera_3d_angles", obj->camera3d_angles);
				}
				else
				{
					reader.Read("ed_camera_2d", obj->camera2d_pos);
					reader.Read("ed_camera_2d_zoom", obj->camera2d_zoom);
				}
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

	MetaData::scene = this;

	for (auto obj : objects)
	{
		obj->GetMetaData()->Prepare(obj);
		obj->GetMetaData()->PostLoad();

		obj->ApplyProperties();

		for (auto comp : obj->components)
		{
			comp->GetMetaData()->Prepare(comp);
			comp->GetMetaData()->PostLoad();

			comp->ApplyProperties();
		}
	}
}

void Scene::Load(const char* name)
{
	JSONReader reader;
	
	StringUtils::GetPath(name, scene_path);
	core.files.AddRootPath(this, scene_path);
	core.files.SetActivePath(this);

	if (reader.Parse(name))
	{
		StringUtils::GetFileName(name, scene_name);
		StringUtils::RemoveExtension(scene_name);

		reader.Read("uid", uid);
		reader.Read("camera3d_angles", camera3d_angles);
		reader.Read("camera3d_pos", camera3d_pos);
		reader.Read("camera_pos", camera2d_pos);
		reader.Read("camera_zoom", camera2d_zoom);
		reader.Read("move_mode", move_mode);
		reader.Read("gizmo2d_align_x", gizmo2d_align_x);
		reader.Read("gizmo2d_align_y", gizmo2d_align_y);
		Load(reader, assets, "SceneAsset", true);
		Load(reader, objects, "SceneObject", false);

#ifdef EDITOR
		if (load_asset_inst)
		{
			while (reader.EnterBlock("asset_instances"))
			{
				uint32_t uid = 0;

				reader.Read("asset_uid", uid);

				SceneAsset* asset = (SceneAsset*)FindByUID(uid, 0, true);

				if (asset)
				{
					while (reader.EnterBlock("instances"))
					{
						asset->instances.push_back(SceneAsset::AssetInstance());

						SceneAsset::AssetInstance& inst = asset->instances[asset->instances.size() - 1];

						reader.Read("scene", inst.scene_path);
						reader.Read("inst_uid", inst.inst_uid);
						reader.Read("inst_child_uid", inst.inst_child_uid);
						reader.Read("is_asset", inst.is_asset);

						reader.LeaveBlock();
					}
				}

				reader.LeaveBlock();
			}
		}
#endif
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

		if (obj->Is3DObject() && obj->Trans())
		{
			writer.Write("transform", obj->Trans());
		}
		
		if (obj->UsingOwnCamera())
		{
			if (obj->Is3DObject())
			{
				writer.Write("ed_camera_3d_pos", obj->camera3d_pos);
				writer.Write("ed_camera_3d_angles", obj->camera3d_angles);
			}
			else
			{
				writer.Write("ed_camera_2d", obj->camera2d_pos);
				writer.Write("ed_camera_2d_zoom", obj->camera2d_zoom);
			}
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
		writer.Write("uid", uid);
		writer.Write("camera3d_angles", camera3d_angles);
		writer.Write("camera3d_pos", camera3d_pos);
		writer.Write("camera_pos", camera2d_pos);
		writer.Write("camera_zoom", camera2d_zoom);
		writer.Write("move_mode", move_mode);
		writer.Write("gizmo2d_align_x", gizmo2d_align_x);
		writer.Write("gizmo2d_align_y", gizmo2d_align_y);

		Save(writer, assets, "SceneAsset");
		Save(writer, objects, "SceneObject");

#ifdef EDITOR
		writer.StartArray("asset_instances");

		for (auto obj : objects)
		{
			obj->SaveAssetData(writer);
		}

		for (auto obj : assets)
		{
			obj->SaveAssetData(writer);
		}

		writer.FinishArray();
#endif
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
		return true;
	}

	playing = true;

	script = core.scripts.CreateContext();

	for (auto asset : assets)
	{
		if (!asset->Play())
		{
			return false;
		}
	}

	for (auto object : objects)
	{
		if (!object->Play())
		{
			return false;
		}

		if (object->group_name.c_str())
		{
			AddToGroup(object, object->group_name.c_str());
		}

		for (auto comp : object->components)
		{
			comp->Play();
		}
	}

	for (auto& item : post_play_list)
	{
		(item.entity->*item.call)();
	}

	post_play_list.clear();

	return true;
}

void Scene::AddPostPlay(int level, Object* entity, Object::DelegateSimple call)
{
	post_play_list.push_back(PostPlayDelegate());

	PostPlayDelegate& delegate = post_play_list.back();

	delegate.level = level;
	delegate.entity = entity;
	delegate.call = call;

	int index = (int)post_play_list.size() - 2;

	while (index >= 0 && post_play_list[index].level > level)
	{
		PostPlayDelegate tmp = post_play_list[index + 1];
		post_play_list[index + 1] = post_play_list[index];
		post_play_list[index] = tmp;

		index--;
	}
}

#ifdef EDITOR
void Scene::Export()
{
	for (auto* asset : assets)
	{
		asset->Export();
	}

	for (auto* object : objects)
	{
		object->Export();
	}
}
#endif

bool Scene::Playing()
{
	return playing;
}

void Scene::EnableTasks(bool enable)
{
	taskPool->SetActive(enable);
	renderTaskPool->SetActive(enable);
}

#ifdef EDITOR
bool Scene::DependFromScene(Scene* scene)
{
	if (scene == this)
	{
		return true;
	}

	for (auto& incl : inc_scenes)
	{
		if (incl->DependFromScene(scene))
		{
			return true;
		}
	}

	return false;
}
#endif

void Scene::GenerateUID(SceneObject* obj, bool is_asset)
{
	union UID32
	{
		uint32_t id;
		struct
		{
			uint16_t id1;
			uint16_t id2;
		};
	};

	UID32 obj_uid;
	obj_uid.id = 0;

	while (!obj_uid.id)
	{
		float koef = Math::Rand() * 0.99f;
		obj_uid.id = (uint32_t)(koef * 1048576);
		obj_uid.id1 |= uid;
		obj_uid.id = FindByUID(obj_uid.id, 0, is_asset) ? 0 : obj_uid.id;
	}

	obj->SetUID(obj_uid.id);
}

SceneObject* Scene::FindInGroup(const char* group_name, const char* name)
{
	vector<Group*> out_groups;

	GetGroup(out_groups, group_name);

	for (auto group : out_groups)
	{
		for (auto object : group->objects)
		{
			if (StringUtils::IsEqual(object->GetName(), name))
			{
				return object;
			}
		}
	}

	return nullptr;
}

void Scene::GetGroup(vector<Group*>& out_groups, const char* name)
{
	if (groups.find(name) != groups.end())
	{
		out_groups.push_back(&groups[name]);
	}

	for (auto& incl : inc_scenes)
	{
		incl->GetGroup(out_groups, name);
	}
}

void Scene::AddToGroup(SceneObject* obj, const char* name)
{
	Group& group = groups[name];

	if (group.name.c_str()[0] == 0)
	{
		group.name = name;
	}

	group.objects.push_back(obj);
}

void Scene::DelFromGroup(Group& group, SceneObject* obj, Scene* new_scene)
{
	for (int i = 0; i < group.objects.size(); i++)
	{
		if (group.objects[i] == obj)
		{
			if (new_scene)
			{
				new_scene->AddToGroup(obj, group.name.c_str());
			}
			group.objects.erase(group.objects.begin() + i);
			break;
		}
	}
}

void Scene::DelFromGroup(SceneObject* obj, const char* name)
{
	DelFromGroup(groups[name], obj);
}

void Scene::DelFromAllGroups(SceneObject* obj, Scene* new_scene)
{
	for (auto& group : groups)
	{
		DelFromGroup(group.second, obj, new_scene);
	}
}

void Scene::Release()
{
	Clear();

	RELEASE(script)

	delete taskPool;
	core.render.DelTaskPool(renderTaskPool);

	delete this;
}