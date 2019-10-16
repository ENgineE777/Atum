#pragma once
#include "ScriptContext.h"
#include "Support/StringUtils.h"
#include "Support/Vector2.h"
#include <vector>

class ScriptCore
{
public:

	class Render
	{
	public:
		void DebugText(float x, float y, string& text);
		void DebugLine2DScr(float x1, float y1, float x2, float y2);
		static void DebugLine2D(Vector2 p1, Vector2 p2);
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

	class Sound
	{
	public:
		class SoundInstance* CreateSound(string& file_name);
		class SoundStream* CreateStream(string& file_name);
	};

	class Utils
	{
	public:
		vector<Vector2> polygon;
		
		Utils()
		{
			polygon.reserve(4);
		}

		int IsPointInTriangle(Vector2& pt, Vector2& p1, Vector2& p2, Vector2& p3);
		int IsPointInRectangle(Vector2& pt, Vector2& start, Vector2& offset, Vector2& size, float angle);
		int IsPointInSector(Vector2& pt, Vector2& center, float orientation, float distance, float angle);

		inline void DrawPolygon(vector<Vector2>& polygon)
		{
			for (int i = 0; i < polygon.size(); i++)
			{
				Render::DebugLine2D(polygon[i], polygon[(i + 1) % polygon.size()]);
			}
		}
	};

	Controls controls;
	Render render;
	Scene scene;
	Sound sound;
	Utils utils;

	void Log(string& text);

	void Register(asIScriptEngine* engine);
};