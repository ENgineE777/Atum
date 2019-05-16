#pragma once
#include "ScriptContext.h"
#include "support/StringUtils.h"
#include "Support/Vector2.h"

class ScriptCore
{
public:

	class Render
	{
	public:
		void DebugText(float x, float y, string& text);
		void DebugLine2D(float x1, float y1, float x2, float y2);
		int GetWidth();
		int GetHeight();
	};

	class Controls
	{
	public:
		int GetAliasIndex(string& alias);
		int GetState(int alias, int action);
		float GetValue(int alias, int delta);
		int GetDebugState(string& alias, int action);
		int IsGamepadConnected();
	};

	class Scene
	{
	public:
		void SetStateToGroup(string& group_name, int state);
		void Load(string& scene_name);
		void Unload(string& scene_name);
		int  Raycast2D(float origin_x, float origin_y, float dir_x, float dir_y, float dist, int group, float& hit_x, float& hit_y, float& normal_x, float& normal_y, string& object, int& index);
		void CallClassInstancesMethod(string& scene_name, string& class_name, string& method);
	};

	class Utils
	{
	public:
		int IsPointInTriangle(Vector2 pt, Vector2 p1, Vector2 p2, Vector2 p3);
	};

	Controls controls;
	Render render;
	Scene scene;
	Utils utils;

	void Log(string& text);

	void Register(asIScriptEngine* engine);
};