
#include "Sprite.h"

void Sprite::Data::Load()
{
	RELEASE(texture);
	texture = render.LoadTexture(tex_name.c_str());
	width = texture ? texture->GetWidth() : 0;
	height = texture ? texture->GetHeight() : 0;
}

void Sprite::Load(JSONReader* loader, Sprite::Data* sprite)
{
	if (loader->EnterBlock("slice"))
	{
		loader->Read("width", sprite->width);
		loader->Read("height", sprite->height);
		loader->Read("type", (int&)sprite->type);

		loader->Read("texName", sprite->tex_name);
		loader->Read("texMode", (int&)sprite->mode);
		loader->Read("texFilter", (int&)sprite->filter);

		int count = GetRectCount(sprite->type);

		for (int i = 0; i<count; i++)
		{
			Rect& rect = sprite->rects[i];

			loader->EnterBlock("Rect");

			loader->Read("pos", rect.pos);
			loader->Read("size", rect.size);
			loader->Read("uv", rect.uv);
			loader->Read("duv", rect.duv);

			loader->LeaveBlock();
		}

		loader->LeaveBlock();

		sprite->Load();
	}
}

void Sprite::Save(JSONWriter* saver, Sprite::Data* sprite)
{
	saver->StartBlock("slice");

	saver->Write("width", sprite->width);
	saver->Write("height", sprite->height);
	saver->Write("type", sprite->type);

	saver->Write("texName", sprite->tex_name.c_str());
	saver->Write("texMode", sprite->mode);
	saver->Write("texFilter", sprite->filter);

	saver->StartArray("Rect");

	int count = GetRectCount(sprite->type);

	for (int i = 0; i<count; i++)
	{
		Rect& rect = sprite->rects[i];

		saver->StartBlock(nullptr);

		saver->Write("pos", rect.pos);
		saver->Write("size", rect.size);
		saver->Write("uv", rect.uv);
		saver->Write("duv", rect.duv);

		saver->FinishBlock();
	}

	saver->FinishArray();

	saver->FinishBlock();
}

void Sprite::Copy(Sprite::Data* src, Sprite::Data* dest)
{
	dest->width = src->width;
	dest->height = src->height;
	dest->type = src->type;

	dest->mode = src->mode;
	dest->filter = src->filter;
	dest->tex_name = src->tex_name;

	dest->texture = render.LoadTexture(dest->tex_name.c_str());

	int count = 1;

	if (dest->type == 1 || dest->type == 2)
	{
		count = 3;
	}
	else
	if (dest->type == 3)
	{
		count = 9;
	}

	for (int i = 0; i<count; i++)
	{
		dest->rects[i] = src->rects[i];
	}
}

void Sprite::Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, Quad* quad)
{
	if (!sprite->texture) return;

	sprite->texture->SetAdress(sprite->mode);
	sprite->texture->SetFilters(sprite->filter, sprite->filter);

	Vector2 pos = trans->offset * trans->size * -1.0f;

	if (sprite->type == Image)
	{
		Rect& rect = sprite->rects[0];
		quad->Draw(sprite->texture, trans->local_trans, pos, trans->size, rect.uv, rect.duv);
	}
	else
	if (sprite->type == ThreeSlicesVert)
	{
		float y[] = { 0, sprite->rects[0].size.y, trans->size.y - sprite->rects[2].size.y, trans->size.y };

		for (int i = 0; i < 3; i++)
		{
			Rect& rect = sprite->rects[i];
			quad->Draw(sprite->texture, trans->local_trans, pos + Vector2(0.0f, y[i]), Vector2(trans->size.x, y[i+1] - y[i]), rect.uv, rect.duv);
		}
	}
	else
	if (sprite->type == ThreeSlicesHorz)
	{
		float x[] = { 0, sprite->rects[0].size.x, trans->size.x - sprite->rects[2].size.x, trans->size.x };

		for (int i = 0; i < 3; i++)
		{
			Rect& rect = sprite->rects[i];
			quad->Draw(sprite->texture, trans->local_trans, pos + Vector2(x[i], 0.0f), Vector2(x[i + 1] - x[i], trans->size.y), rect.uv, rect.duv);
		}
	}
	else
	if (sprite->type == NineSlices)
	{
		float x[] = { 0, sprite->rects[0].size.x, trans->size.x - sprite->rects[2].size.x, trans->size.x };
		float y[] = { 0, sprite->rects[0].size.y, trans->size.y - sprite->rects[8].size.y, trans->size.y };

		int index = 0;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				Rect& rect = sprite->rects[index];
				quad->Draw(sprite->texture, trans->local_trans, pos + Vector2(x[j], y[i]), Vector2(x[j + 1] - x[j], y[i + 1] - y[i]), rect.uv, rect.duv);
				index++;
			}
	}
}



