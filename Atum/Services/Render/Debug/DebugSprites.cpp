#include "DebugSprites.h"
#include "DebugPrograms.h"

void DebugSprites::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float2, VertexDecl::Texcoord, 0 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(2, desc);

	debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugSprites::Draw);

	vbuffer = core.render.GetDevice()->CreateBuffer(sizeof(SpriteVertex) * 4, sizeof(SpriteVertex));

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

	prg = core.render.GetProgram("DbgSprite");
}

void DebugSprites::AddSprite(Texture* texture, Vector2 pos, Vector2 size, Vector2 offset, float angle, Color color)
{
	if (sprites.size()>1000) return;

	sprites.push_back(Sprite());
	Sprite* spr = &sprites[sprites.size()-1];

	spr->texture = texture;
	spr->color = color;
	spr->pos = pos;
	spr->size = size;
	spr->offset = offset;
	spr->angle = angle;
}

void DebugSprites::Draw(float dt)
{
	if (sprites.size() == 0) return;

	core.render.GetDevice()->SetVertexDecl(vdecl);
	core.render.GetDevice()->SetVertexBuffer( 0, vbuffer);

	core.render.GetDevice()->SetProgram(prg);
	Vector4 params[3];

	for (auto& sprite : sprites)
	{
		prg->SetTexture(Shader::Type::Pixel, "diffuseMap", sprite.texture ? sprite.texture : core.render.GetWhiteTexture());
		prg->SetVector(Shader::Type::Pixel, "color", (Vector4*)&sprite.color.r, 1);

		params[0] = Vector4((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight(), 0, 0);

		params[1] = Vector4(sprite.pos.x, sprite.pos.y, sprite.size.x, sprite.size.y);

		params[2] = Vector4(sprite.offset.x, sprite.offset.y, sprite.angle, 0.0f);

		prg->SetVector(Shader::Type::Vertex, "desc", params, 3);

		core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
	}

	sprites.clear();
}

void DebugSprites::Release()
{
	RELEASE(vbuffer)
}
