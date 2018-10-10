
#pragma once

#include <math.h>

struct Vec2
{
	int x;
	int y;

	Vec2();
	Vec2(int set_x, int set_y);
	bool operator == (const Vec2& vec);
	Vec2 GetDelta(Vec2& vec);
};

inline Vec2::Vec2()
{
	x = 0;
	y = 0;
}

inline Vec2::Vec2(int set_x, int set_y)
{
	x = set_x;
	y = set_y;
}

inline bool Vec2::operator == (const Vec2& vec)
{
	return (x == vec.x && y == vec.y);
}

inline Vec2 Vec2::GetDelta(Vec2& vec)
{
	return Vec2(fabs(x - vec.x), fabs(y - vec.y));
}

inline Vec2 operator + (const Vec2& l, const Vec2& r)
{
	return Vec2(l.x + r.x, l.y + r.y);
}
