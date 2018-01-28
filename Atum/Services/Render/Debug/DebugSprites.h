
#pragma once

#include "Services/Render/Render.h"

class DebugSprites : public Object
{
	struct SpriteVertex
	{
		Vector  Position;
		Vector2 Texcoords;
	};

	GeometryBuffer* vbuffer;

	struct Sprite
	{
		Texture* texture;
		Vector2 pos;
		Vector2 size;
	};

	std::vector<Sprite> sprites; 

public:

	void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
	void AddSprite(Texture* texture, Vector2& pos, Vector2& size);
	void Draw(float dt);
	void Release();
};

