
#pragma once

#include "Services/Render/Device.h"
#include "Services/Render/Program.h"
#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>
#include <map>

class Render
{
	friend class TextureDX11;

	Device* device;
	Matrix trans[4];
	bool   need_calc_trans;

	struct TextureRef
	{
		int count;
		Texture* texture;
	};

	std::map<std::string, TextureRef> textures;

	class DebugLines*  lines;
	class DebugSpheres* spheres;
	class DebugBoxes* boxes;

	TaskExecutor::GroupTaskPool* groupTaskPool;
	TaskExecutor::SingleTaskPool* debugTaskPool;

public:

	enum Transform
	{
		World = 0,
		View = 1,
		Projection,
		WrldViewProj
	};

	Render();

	bool Init(const char* device, int width, int height, void* data);

	Device* GetDevice();

	void SetTransform(Transform trans, Matrix& mat);
	void GetTransform(Transform trans, Matrix& mat);

	Texture* LoadTexture(const char* name);

	TaskExecutor::SingleTaskPool* AddTaskPool();
	void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

	void Execute(float dt);

	void AddExecutedLevelPool(int level);
	void ExecutePool(int level, float dt);

	void DebugLine(Vector& from, Color& from_clr, Vector& to, Color& to_clr, bool use_depth = true);
	void DebugSphere(Vector& pos, Color& color, float radius);
	void DebugBox(Matrix& pos, Color& color, Vector& scale);

	void Release();

protected:
	void CalcTrans();
	bool TexRefIsEmpty(Texture* texture);
};

extern Render render;