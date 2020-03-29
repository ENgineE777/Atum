
#pragma once

#include "Services/Render/Render.h"
#include "Services/TaskExecutor/TaskExecutor.h"

class Mesh
{
public:

	class Instance : public Object
	{
		Mesh* res = nullptr;
		TaskExecutor::SingleTaskPool* taskPool;

	public:

		Matrix transform;
		Color color;
		int visible = 1;

		void Init(Mesh* mesh, TaskExecutor::SingleTaskPool* taskPool);

		Vector3 GetBBMin();
		Vector3 GetBBMax();

		int GetSubMeshesCount();

		Vector3 GetBBMin(int sub_mesh_index);
		Vector3 GetBBMax(int sub_mesh_index);

		void Render(float dt);
		void ShRender(float dt);
		void Render(Program* prg);

		void GetLocatorTransform(const char* name, Matrix& loc_transform);

		void Release();
	};

	struct MeshVertex
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 normal;
	};

	struct SubMesh
	{
		int texture = -1;
		int num_triangles;
		DataBuffer* indices;
		DataBuffer* vertices;

		Vector3 bb_max = -FLT_MAX;
		Vector3 bb_min = FLT_MAX;
	};

	VertexDecl* vdecl;

	Vector3 bb_max = -FLT_MAX;
	Vector3 bb_min = FLT_MAX;

	std::vector<Texture*> textures;
	std::vector<SubMesh> meshes;

	map<string, Matrix> locators;

	bool Load(const char* filename);
	bool LoadFBX(const char* filename);

	void Release();
};