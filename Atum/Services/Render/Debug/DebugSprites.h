
#pragma once

#include "Services/Render/Render.h"

class DebugSprites : public Object
{
	struct SpriteVertex
	{
		Vector  Position;
		Vector2 Texcoords;
	};

	Program*        prg;
	VertexDecl*     vdecl;
	GeometryBuffer* vbuffer;

	struct Sprite
	{
		Texture* texture;
		Color    color;
		Vector2  pos;
		Vector2  size;
	};

	Texture* whiteTex = nullptr;
	std::vector<Sprite> sprites; 

public:

	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddSprite(Texture* texture, Vector2& pos, Vector2& size, Color& color);
	void Draw(float dt);
	void Release();
};

