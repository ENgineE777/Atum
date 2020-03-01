#include "Math.h"

namespace Math
{
	float Rand()
	{
		return ((float)rand() / RAND_MAX);
	}

	float RandRange(float a, float b)
	{
		return (a + (b - a) * (float)rand() / RAND_MAX);
	}

	int IsPointInPolygon(Vector2& pt, vector<Vector2>& polygon)
	{
		vector<Vector2> temp;

		int count = (int)polygon.size();
		temp.resize(count);

		for (int i = 0; i < count; i++)
		{
			temp[i] = polygon[i] - pt;
		}

		bool orientation = temp[0].Cross(temp[1]) > 0;

		for (int i = 1; i < count; i++)
		{
			if ((temp[i].Cross(temp[(i + 1) % count]) > 0) != orientation)
			{
				return 0;
			}
		}

		return 1;
	}

	float Clamp(float value, float a, float b)
	{
		return fmin(fmax(value, a), b);
	}

	float Sign(float value)
	{
		return value > 0.000001f ? 1.0f : -1.0f;
	}

	float AdvanceAngle(float angle, float target_angle, float delta_angle)
	{
		angle = angle - (angle > PI ? TwoPI : 0.0f);
		target_angle = target_angle - (target_angle > PI ? TwoPI : 0.0f);

		if (target_angle - angle > PI)
		{
			target_angle -= TwoPI;
		}
		else
		if (angle - target_angle > PI)
		{
			target_angle += TwoPI;
		}

		if (fabs(target_angle - angle) < fabs(delta_angle))
		{
			angle = target_angle;
		}
		else
		{
			angle += delta_angle * Sign(target_angle - angle);
		}

		if (angle > PI)
		{
			angle -= TwoPI;
		}
		else
		if (angle < PI)
		{
			angle += TwoPI;
		}

		return angle;
	}
}