
#pragma once

#include "Services/Render/Render.h"

class Font;

class DebugFont : public Object
{
	Font* font;
#if PLATFORM_ANDROID
	int font_size = 20;
#else
	int font_size = 11;
#endif

	struct Text
	{
		Vector3 pos;
		ScreenCorner corner;
		float dist;
		Color color;
		char text[256];
	};

	std::vector<Text> texts;
	std::vector<Text> textsIn3d;

public:
	
	DebugFont();
	virtual ~DebugFont();

	virtual bool Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddText(std::vector<Text>& texts, Vector3 pos, ScreenCorner corner, float dist, Color color, const char* text);
	void AddText(Vector2 pos, ScreenCorner corner, Color color, const char* text);
	void AddText(Vector3 pos, float dist, Color color, const char* text);
	void Draw(float dt);
	void Release();
};
