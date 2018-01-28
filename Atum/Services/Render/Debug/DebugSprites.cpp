#include "DebugSprites.h"
#include "DebugPrograms.h"

void DebugSprites::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugSprites::Draw);

	vbuffer = render.GetDevice()->CreateBuffer(sizeof(SpriteVertex) * 4, sizeof(SpriteVertex));

	SpriteVertex* Data = (SpriteVertex*)vbuffer->Lock();

	Data[0].Position.x = 0.0f;
	Data[1].Position.x = 0.0f;
	Data[2].Position.x = 1.0f;
	Data[3].Position.x = 1.0f;

	Data[0].Position.y = 1.0f;
	Data[1].Position.y = 0.0f;
	Data[2].Position.y = 1.0f;
	Data[3].Position.y = 0.0f;

	Data[0].Position.z = 0.0f;
	Data[1].Position.z = 0.0f;
	Data[2].Position.z = 0.0f;
	Data[3].Position.z = 0.0f;

	Data[0].Texcoords.x = 0.0f;
	Data[1].Texcoords.x = 0.0f;
	Data[2].Texcoords.x = 1.0f;
	Data[3].Texcoords.x = 1.0f;

	Data[0].Texcoords.y = 1.0f;
	Data[1].Texcoords.y = 0.0f;
	Data[2].Texcoords.y = 1.0f;
	Data[3].Texcoords.y = 0.0f;

	vbuffer->Unlock();
}

void DebugSprites::AddSprite(Texture* texture, Vector2& pos, Vector2& size)
{
	if (sprites.size()>1000) return;

	sprites.push_back(Sprite());
	Sprite* spr = &sprites[sprites.size()-1];

	spr->texture = texture;
	
	spr->pos = pos;
	spr->size = size;
}

void DebugSprites::Draw(float dt)
{
	if (sprites.size() == 0) return;

	render.GetDevice()->SetVertexBuffer( 0, vbuffer);

	DebugPrograms::sprite_prg->Apply();
	Vector4 params[2];

	for (int i=0;i<sprites.size();i++)
	{
		DebugPrograms::sprite_prg->PS_SetTexture("diffuseMap", sprites[i].texture);

		params[0] = Vector4((float)render.GetDevice()->GetWidth(),
		                    (float)render.GetDevice()->GetHeight(), 0, 0);

		params[1] = Vector4(sprites[i].pos.x,sprites[i].pos.y,
		                    sprites[i].size.x,sprites[i].size.y);

		DebugPrograms::sprite_prg->VS_SetVector("desc", params, 2);

		render.GetDevice()->Draw(Device::TriangleStrip, 0, 2 );
	}

	sprites.clear();
}

void DebugSprites::Release()
{
	RELEASE(vbuffer)
}