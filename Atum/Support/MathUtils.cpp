#include "MathUtils.h"

namespace MathUtils
{
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
}