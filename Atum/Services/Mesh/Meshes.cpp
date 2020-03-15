
#include "Services/Core/Core.h"

void Meshes::Init()
{
}

Mesh::Instance* Meshes::LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool)
{
	Mesh* mesh = nullptr;

	if (meshes.count(name) > 0)
	{
		MeshRef& ref = meshes[name];

		ref.count++;
		mesh = ref.mesh;
	}
	else
	{
		mesh = new Mesh();
		mesh->Load(name);

		MeshRef& ref = meshes[name];

		ref.count = 1;
		ref.mesh = mesh;
	}

	Mesh::Instance* instance = new Mesh::Instance();
	instance->Init(mesh, taskPool);

	return instance;
}

bool Meshes::DecRef(Mesh* mesh)
{
	typedef std::map<std::string, MeshRef>::iterator it_type;

	for (it_type iterator = meshes.begin(); iterator != meshes.end(); iterator++)
	{
		if (iterator->second.mesh == mesh)
		{
			iterator->second.count--;

			if (iterator->second.count == 0)
			{
				meshes.erase(iterator);
				return true;
			}

			return false;
		}
	}

	return true;
}