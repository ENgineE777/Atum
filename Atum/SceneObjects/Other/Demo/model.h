
#pragma once

#include "Services/Render/Render.h"
#include "Services/TaskExecutor/TaskExecutor.h"

class Model
{
public:

	class Drawer : public Object
	{
		Model* res;
		Matrix world;
		Vector4 color;
		bool    show;

	public:
		Drawer()
		{
			res = NULL;
			color = 1.0f;
			show = true;
		};

		void Init(Model* model, TaskExecutor::SingleTaskPool* taskPool);
		void SetColor(Vector4& color);
		void SetPosition(Vector2 pos);
		void SetTransform(Matrix& mat);
		void Show(bool set);

		void Render(float dt);
		void ShRender(float dt);
		void Render(Program* prg);
	};

	struct ModelVertex
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 normal;
	};

	struct Mesh
	{
		int texture;
		int num_triangles;
		DataBuffer* buffer;
	};

	VertexDecl* vdecl;

	Vector3 bb_max;
	Vector3 bb_min;

	Matrix world;

	std::vector<Texture*> textures;
	std::vector<Mesh> meshes;

	Vector3 locator;

	void LoadModelMS3D(const char* filename);
};