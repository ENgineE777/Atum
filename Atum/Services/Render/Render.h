
#pragma once

#include "Services/Render/Device.h"
#include "Services/Render/Program.h"
#include "Services/TaskExecutor/TaskExecutor.h"
#include <vector>
#include <map>

/**
\ingroup gr_code_services_render
*/

/**
\brief PhysScene

This is representation of a physical scene. Objecte to a scene are adding via proper
methods of PhysScene.

*/

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

	class DebugLines*       lines;
	class DebugSpheres*     spheres;
	class DebugBoxes*       boxes;
	class DebugTriangles*   triangles;
	class DebugFont*        font;
	class DebugSprites*     sprites;
	class DebugTriangles2D* triangles2D;

	TaskExecutor::GroupTaskPool* groupTaskPool;
	TaskExecutor::SingleTaskPool* debugTaskPool;

public:

	enum Transform
	{
		World = 0 /*!< Hardware button in pressed state */,
		View /*!< Hardware button in pressed state */,
		Projection /*!< Hardware button in pressed state */,
		WrldViewProj /*!< Hardware button in pressed state */
	};

	Render();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	bool Init(const char* device, void* external_device);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	Device* GetDevice();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void SetTransform(Transform trans, Matrix mat);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void GetTransform(Transform trans, Matrix& mat);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	Program* GetProgram(const char* name);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	Texture* LoadTexture(const char* name);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	TaskExecutor::SingleTaskPool* AddTaskPool();

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void Execute(float dt);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void AddExecutedLevelPool(int level);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void ExecutePool(int level, float dt);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugLine(Vector from, Color from_clr, Vector to, Color to_clr, bool use_depth = true);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugLine2D(Vector2 from, Color from_clr, Vector2 to, Color to_clr);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugRect2D(Vector2 from, Vector2 to, Color color);
	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugSphere(Vector pos, Color color, float radius);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugBox(Matrix pos, Color color, Vector scale);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugTriangle(Vector p1, Vector p2, Vector p3, Color color);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugTriangle2D(Vector2 p1, Vector2 p2, Vector2 p3, Color color);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugPrintText(Vector2 pos, Color color, const char* text, ...);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugPrintText(Vector pos, float dist, Color color, const char* text, ...);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void DebugSprite(Texture* texture, Vector2 pos, Vector2 size, Color color = COLOR_WHITE, Vector2 offset = 0.0f, float angle = 0.0f);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	Vector TransformToScreen(Vector pos, int type);

	/**
	\brief This variable stores position on start and restors it when Reset was clled from script
	*/
	void Release();

protected:
	void CalcTrans();
	bool TexRefIsEmpty(Texture* texture);
};