
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
		bool show = true;

		void Init(Mesh* mesh, TaskExecutor::SingleTaskPool* taskPool);
		void SetPosition(Vector2 pos);

		void Render(float dt);
		void ShRender(float dt);
		void Render(Program* prg);

		void Release();
	};

	struct MeshVertex
	{
		Vector  pos;
		Vector2 uv;
		Vector  normal;
	};

	struct SubMesh
	{
		int texture = -1;
		int num_triangles;
		DataBuffer* indices;
		DataBuffer* vertices;
	};

	VertexDecl* vdecl;

	Vector bb_max;
	Vector bb_min;

	Matrix world;

	std::vector<Texture*> textures;
	std::vector<SubMesh> meshes;

	Vector locator;

	bool Load(const char* filename);
	bool LoadFBX(const char* filename);

	void Release();
};