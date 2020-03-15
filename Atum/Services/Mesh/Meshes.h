
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include <map>
#include <string>
#include "Support/StringUtils.h"
#include "MeshRes.h"

class Meshes
{
	struct MeshRef
	{
		int count;
		Mesh* mesh;
	};

	std::map<std::string, MeshRef> meshes;

public:

	void Init();
	Mesh::Instance* LoadMesh(const char* name, TaskExecutor::SingleTaskPool* taskPool);
	bool DecRef(Mesh* mesh);
};
