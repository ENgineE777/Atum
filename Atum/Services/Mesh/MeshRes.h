
#pragma once

#include "Services/Render/Render.h"
#include "Services/TaskExecutor/TaskExecutor.h"

class Mesh
{
public:

	class Instance : public Object
	{
		Mesh* res;
		Matrix world;
		Vector4 color;
		bool    show;

	public:
		Instance()
		{
			res = NULL;
			color = 1.0f;
			show = true;
		};

		void Init(Mesh* mesh, TaskExecutor::SingleTaskPool* taskPool);
		void SetColor(Vector4& color);
		void SetPosition(Vector2 pos);
		void SetTransform(Matrix& mat);
		void Show(bool set);

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

	void Load(const char* filename);
	void LoadFBX(const char* filename);

	void Release();
};