
#pragma once

#include "Services/Render/Device.h"
#include "Services/Render/Program.h"
#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>
#include <map>

class Render
{
	friend class TextureDX11;
	friend class TextureGLES;

	Device* device;
	Matrix trans[4];
	bool   need_calc_trans;

	struct TextureRef
	{
		int count;
		Texture* texture;
	};

	std::map<std::string, TextureRef> textures;

	std::map<std::string, Program*> programs;

	class DebugLines*     lines;
	class DebugSpheres*   spheres;
	class DebugBoxes*     boxes;
	class DebugTriangles* triangles;
	class DebugFont*      font;
	class DebugSprites*   sprites;

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

	void SetTransform(Transform trans, Matrix mat);
	void GetTransform(Transform trans, Matrix& mat);

	Program* GetProgram(const char* name);

	Texture* LoadTexture(const char* name);

	TaskExecutor::SingleTaskPool* AddTaskPool();
	void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

	void Execute(float dt);

	void AddExecutedLevelPool(int level);
	void ExecutePool(int level, float dt);

	void DebugLine(Vector from, Color from_clr, Vector to, Color to_clr, bool use_depth = true);
	void DebugLine2D(Vector2 from, Color from_clr, Vector2 to, Color to_clr);
	void DebugSphere(Vector pos, Color color, float radius);
	void DebugBox(Matrix pos, Color color, Vector scale);
	void DebugTriangle(Vector p1, Vector p2, Vector p3, Color color);
	void DebugPrintText(Vector2 pos, Color color, const char* text, ...);
	void DebugPrintText(Vector pos, float dist, Color color, const char* text, ...);
	void DebugSprite(Texture* texture, Vector2 pos, Vector2 size, Color color = COLOR_WHITE);

	Vector TransformToScreen(Vector pos, int type);

	void Release();

protected:
	void CalcTrans();
	bool TexRefIsEmpty(Texture* texture);
};

extern Render render;