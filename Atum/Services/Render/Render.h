
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
\brief Render

This is main class of render system.

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

	Texture* white_tex = nullptr;

public:

	enum Transform
	{
		World = 0 /*!< World trasformation */,
		View /*!< View trasformation */,
		Projection /*!< Projection trasformation */,
		WrldViewProj /*!< Final transformation */
	};

#ifndef DOXYGEN_SKIP

	Render();

	bool Init(const char* device, void* external_device);
	void Execute(float dt);
	void Release();
#endif

	/**
	\brief Get pointer to a device interface

	\return Pointer to a device interface
	*/
	Device* GetDevice();

	/**
	\brief Set transform

	\param[in] trans Type of transform
	\param[in] mat Trnasform itself
	*/
	void SetTransform(Transform trans, Matrix mat);

	/**
	\brief Get transform

	\param[in] trans Type of transform
	\param[out] mat Trnasform itself
	*/
	void GetTransform(Transform trans, Matrix& mat);

	/**
	\brief Get pointer to a program by name

	\param[in] name Name of a program

	\return Pointer to a program
	*/
	Program* GetProgram(const char* name);

	/**
	\brief Load texture

	\param[in] name Full path to a texture

	\return Pointer to a texture
	*/
	Texture* LoadTexture(const char* name);

	/**
	\brief Creates new task pool in group render task pool

	\return Pointer to a task pool
	*/
	TaskExecutor::SingleTaskPool* AddTaskPool();

	/**
	\brief Deletes task pool from a group render task pool

	\param[in] pool Pointer to a task pool
	*/
	void DelTaskPool(TaskExecutor::SingleTaskPool* pool);

	/**
	\brief Add execution level in group task pool

	\param[in] level Level of execution
	*/
	void AddExecutedLevelPool(int level);

	/**
	\brief Execute all tasks in group task pool with particula level of execution

	\param[in] level Level of execution
	\param[in] dt Deletaime since last frame
	*/
	void ExecutePool(int level, float dt);

	/**
	\brief Draw debug line in 3D space

	\param[in] from Position of first point
	\param[in] from_clr Color of first color
	\param[in] to Position of second point
	\param[in] to_clr Color of second point
	\param[in] use_depth Should depth be ignored

	*/
	void DebugLine(Vector3 from, Color from_clr, Vector3 to, Color to_clr, bool use_depth = true);

	/**
	\brief Draw debug line in 2D space

	\param[in] from Position of first point
	\param[in] from_clr Color of first color
	\param[in] to Position of second point
	\param[in] to_clr Color of second point

	*/
	void DebugLine2D(Vector2 from, Color from_clr, Vector2 to, Color to_clr);

	/**
	\brief Draw rectangle constided from lines in 2D space

	\param[in] from Left top point
	\param[in] to Right bottom point
	\param[in] color Color of a rectangle

	*/
	void DebugRect2D(Vector2 from, Vector2 to, Color color);

	/**
	\brief Draw debug sphere in 3D space

	\param[in] pos Postion of a sphere
	\param[in] color Color of a sphere
	\param[in] radius Radius of a sphere

	*/
	void DebugSphere(Vector3 pos, Color color, float radius, bool full_shade = true);

	/**
	\brief Draw debug box in 3d space

	\param[in] pos Transformation of a box
	\param[in] color Color of a box
	\param[in] scale Size of a box

	*/
	void DebugBox(Matrix pos, Color color, Vector3 scale);

	/**
	\brief Draw debug triangle in 3D space

	\param[in] p1 First point of a triangle
	\param[in] p2 Second point of a triangle
	\param[in] p3 Third point of a triangle
	\param[in] color Color of a trinagle

	*/
	void DebugTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color);

	/**
	\brief Draw debug triangle in 2D space

	\param[in] p1 First point of a triangle
	\param[in] p2 Second point of a triangle
	\param[in] p3 Third point of a triangle
	\param[in] color Color of a trinagle

	*/
	void DebugTriangle2D(Vector2 p1, Vector2 p2, Vector2 p3, Color color);

	/**
	\brief Print debug text in 2D space

	\param[in] pos Postion of a text in screen space
	\param[in] color Color of a text
	\param[in] text Text format string of a text

	*/
	void DebugPrintText(Vector2 pos, ScreenCorner corner, Color color, const char* text, ...);

	/**
	\brief Print debug text in 2D space

	\param[in] pos Postion of a text in world space
	\param[in] abs_unit Is position in absolute units
	\param[in] color Color of a text
	\param[in] text Text format string of a text

	*/
	void DebugPrintText(Vector2 pos, ScreenCorner corner, bool abs_unit, Color color, const char* text, ...);

	/**
	\brief Print debug text in 3D space

	\param[in] pos Postion of a text
	\param[in] dist Distance to a camera in which text is visible 
	\param[in] color Color of a text
	\param[in] text Text format string of a text

	*/
	void DebugPrintText(Vector3 pos, float dist, Color color, const char* text, ...);

	/**
	\brief Draw debug sprite

	\param[in] texture Texture of a sprite
	\param[in] pos Postion of a sprite
	\param[in] size Size of a sprite
	\param[in] color Color of a sprite
	\param[in] offset Offset of local origin
	\param[in] angle Rotation around local origin

	*/
	void DebugSprite(Texture* texture, Vector2 pos, Vector2 size, Color color = COLOR_WHITE, Vector2 offset = 0.0f, float angle = 0.0f);

	/**
	\brief Get pointer to texture filled by white color

	\return Pointer to texture filled by white color
	*/
	inline Texture* GetWhiteTexture()
	{
		return white_tex;
	}

	/**
	\brief Transform pount to a screen

	\param[in] pos Position of a point
	\param[in] type Type of transform. 0 - transform in post camera space in range -1.0f .. 1.0f. 1 - transform in post camera space in range 0 .. 1.0f. 2 - transform in post camera space in screen coordinates

	\return Result of transformation
	*/
	Vector3 TransformToScreen(Vector3 pos, int type);

protected:
	void CalcTrans();
	bool TexRefIsEmpty(Texture* texture);
};