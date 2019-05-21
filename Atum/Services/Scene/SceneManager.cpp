
#include "SceneManager.h"
#include "SceneObject.h"

void SceneManager::Init()
{
}

void SceneManager::LoadProject(const char* project_name)
{
	StringUtils::GetPath(project_name, project_path);

	JSONReader reader;
	if (reader.Parse(project_name))
	{
		int start_scene = -1;
		if (!reader.Read("start_scene", start_scene))
		{
			return;
		}

		int count = 0;
		reader.Read("scenes_count", count);
		scenes.resize(count);

		int index = 0;
		char name[256];

		while (reader.EnterBlock("scenes"))
		{
			SceneHolder& scn = scenes[index];

			reader.Read("path", scn.path);

			StringUtils::GetFileName(scn.path.c_str(), name);
			StringUtils::RemoveExtension(name);

			scenes_search[name] = &scenes[index];

			while (reader.EnterBlock("include"))
			{
				reader.Read("path", name, 256);

				char incl_name[256];

				StringUtils::GetFileName(name, incl_name);
				StringUtils::RemoveExtension(incl_name);

				scn.included_pathes.push_back(incl_name);

				reader.LeaveBlock();
			}

			reader.LeaveBlock();

			index++;
		}

		for (auto& scn : scenes)
		{
			for (auto& incl : scn.included_pathes)
			{
				scn.included.push_back(scenes_search[incl.c_str()]);
			}
		}

		pscene = core.physics.CreateScene();
		LoadScene(&scenes[start_scene]);
	}
}

void SceneManager::LoadScene(const char* name)
{
	LoadScene(scenes_search[name]);
}

void SceneManager::LoadScene(SceneHolder* holder)
{
	holder->ref_counter++;

	if (holder->scene)
	{
		return;
	}

	for (auto& incl : holder->included)
	{
		LoadScene(incl);
	}

	holder->scene = new Scene();
	holder->scene->Init();

	for (auto& incl : holder->included)
	{
		holder->scene->inc_scenes.push_back(incl->scene);
	}

	char path[1024];
	StringUtils::Printf(path, 1024, "%s%s", project_path, holder->path.c_str());

	holder->scene->Load(path);
	holder->scene->Play();
}

void SceneManager::Execute(float dt)
{
	for (auto* holder : scenes_to_delete)
	{
		UnloadScene(holder);
	}

	scenes_to_delete.clear();

	for (int i = 0; i < scenes.size(); i++)
	{
		auto& scn = scenes[i];

		if (scn.scene)
		{
			if (scn.ref_counter != 0)
			{
				scn.scene->Execute(dt);
			}
		}
	}
}

void SceneManager::SetScenesGroupsState(const char* group_name, int state)
{
	for (auto& scn : scenes)
	{
		if (scn.scene)
		{
			vector<Scene::Group*> out_group;
			scn.scene->GetGroup(out_group, group_name);

			for (auto group : out_group)
			{
				for (auto obj : group->objects)
				{
					obj->SetState((SceneObject::State)state);
				}
			}
		}
	}
}

void SceneManager::UnloadScene(const char* name)
{
	SceneHolder* holder = scenes_search[name];

	if (holder->ref_counter > 0)
	{
		holder->ref_counter--;

		if (holder->ref_counter == 0)
		{
			scenes_to_delete.push_back(holder);
		}
	}
}

void SceneManager::UnloadScene(SceneHolder* holder)
{
	if (holder->ref_counter == 0)
	{
		RELEASE(holder->scene)
	}

	for (auto& incl : holder->included)
	{
		incl->ref_counter--;

		UnloadScene(incl);
	}
}

void SceneManager::UnloadAll()
{
	for (auto& scn : scenes)
	{
		if (scn.scene)
		{
			RELEASE(scn.scene)
		}
	}

	scenes.clear();
	scenes_search.clear();

	if (pscene)
	{
		core.physics.DestroyScene(pscene);
		pscene = nullptr;
	}
}