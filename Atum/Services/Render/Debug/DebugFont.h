
#pragma once

#include "Services/Render/Render.h"

class Font;

class DebugFont : public Object
{
	Font* font;

	struct Text
	{
		Vector pos;
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
	void AddText(std::vector<Text>& texts, Vector pos, float dist, Color color, const char* text);
	void AddText(Vector2 pos, Color color, const char* text);
	void AddText(Vector pos, float dist, Color color, const char* text);
	void Draw(float dt);
	void Release();
};
