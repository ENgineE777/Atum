
#pragma once

#include "Support/Support.h"
#include "SceneObjects/2D/Sprite.h"
#include "Services/Font/Fonts.h"

class EditorDrawer
{
	Transform2D trans;
	Sprite::FrameState state;

public:

	Font* font;
	Texture* anchorn = nullptr;
	Texture* center = nullptr;
	Texture* node_tex = nullptr;
	Texture* arrow_tex = nullptr;

	void Init();
	void DrawSprite(Texture* tex, Vector2 pos, Vector2 size, Color color);
	void PrintText(Vector2 pos, Color color, const char* text);
	void DrawCurve(Vector2 from, Vector2 to, Color color);
};

extern EditorDrawer editor_drawer;
