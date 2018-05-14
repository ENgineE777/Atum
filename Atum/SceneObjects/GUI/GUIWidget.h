
#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"

class GUIWidget
{
public:

	enum SizeType
	{
		SizeFixed,
		SizeContent,
		SizeParent
	};

	static Texture* tex;

	Matrix   pos;
	SizeType sizeTypes[2];
	Vector2  size;
	bool     focused;
	bool     howered;
	std::vector<GUIWidget*> childs;

	GUIWidget();
	virtual void DrawSelf();
	virtual void Draw();
};
