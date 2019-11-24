#pragma once

#include "Vector2.h"
#include "std.h"

namespace MathUtils
{
	int IsPointInPolygon(Vector2& pt, vector<Vector2>& polygon);
	float Clamp(float value, float a, float b);
}