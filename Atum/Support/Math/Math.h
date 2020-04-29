#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Support/std.h"

namespace Math
{
	constexpr static float Radian = 0.01745329f;
	constexpr static float PI = 3.14159265f;
	constexpr static float TwoPI = 6.28318530f;
	constexpr static float HalfPI = 1.57079632f;

	float Rand();
	float RandRange(float a, float b);

	int IsPointInPolygon(Vector2& pt, vector<Vector2>& polygon);
	float Clamp(float value, float a, float b);
	float Sign(float value);
	float AdvanceAngle(float angle, float target_angle, float delta_angle);
	float GetAnglesDifference(float angle, float target_angle);
	bool IsSameAngles(float angle, float target_angle);
	bool IntersectSphereRay(Vector3 pos, float radius, Vector3 start, Vector3 dir);
	bool IntersectBBoxRay(Vector3 vmin, Vector3 vmax, Vector3 start, Vector3 dir);
}