#pragma once
#include "ScriptContext.h"
#include "Support/StringUtils.h"
#include "Support/Math/Math.h"
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
		void DebugSphere(Vector3& pos, Vector3& color, float radius);
		int GetWidth();
		int GetHeight();
		void TransformToScreen(Vector3& pos, Vector3& res, Vector2& left_up, Vector2& right_down);
		void GetRayFromScreen(Vector2& pos, Vector3& origin, Vector3& dir);
	};

	class Controls
	{
	public:
		int GetAliasIndex(string& alias);
		int GetState(int alias, int action);
		float GetValue(int alias, int delta);
		bool GetDebugState(string& alias, int action);
		bool IsGamepadConnected();
	};

	class Scene
	{
	public:
		void SetStateToGroup(string& group_name, int state);
		void Load(string& scene_name);
		void Unload(string& scene_name);
		bool Raycast2D(float origin_x, float origin_y, float dir_x, float dir_y, float dist, int group, float& hit_x, float& hit_y, float& normal_x, float& normal_y, string& object, int& index);
		bool Raycast3D(Vector3& origin, Vector3& dir, float dist, int group, Vector3& hit, Vector3 &normal, string& object, int& index);
		void CallClassInstancesMethod(string& scene_name, string& class_name, string& method);
		void PlayParticles(string& scen_name, string& name, Vector3& pos);
		class ParticleSystem* CreateParticles(string& scen_name, string& name);
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

		bool IsPointInTriangle(Vector2& pt, Vector2& p1, Vector2& p2, Vector2& p3, bool debug);
		bool IsPointInRectangle(Vector2& pt, Vector2& start, Vector2& offset, Vector2& size, float angle, bool debug);
		bool IsPointInSector(Vector2& pt, Vector2& center, float orientation, float distance, float angle, bool debug);

		inline void DrawPolygon(vector<Vector2>& polygon)
		{
			for (int i = 0; i < polygon.size(); i++)
			{
				Render::DebugLine2D(polygon[i], polygon[(i + 1) % polygon.size()]);
			}
		}

		float AdvanceAngle(float angle, float target_angle, float delta_angle);
		bool IsSameAngles(float angle, float target_angle);
		float GetAnglesDifference(float angle, float target_angle);
	};

	Controls controls;
	Render render;
	Scene scene;
	Sound sound;
	Utils utils;

	void Log(string& text);

	void Register(asIScriptEngine* engine);
};