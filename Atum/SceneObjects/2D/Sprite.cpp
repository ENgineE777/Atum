
#include "Sprite.h"
#include "Services/Core/Core.h"

void Sprite::Data::LoadTexture()
{
	RELEASE(texture);
	texture = core.render.LoadTexture(tex_name.c_str());
	width = texture ? texture->GetWidth() : 0;
	height = texture ? texture->GetHeight() : 0;
}

bool Sprite::use_ed_cam = true;
Vector2 Sprite::cam_pos = 0.0f;
Vector2 Sprite::ed_cam_pos = 0.0f;
float Sprite::ed_cam_zoom = 1.0f;
Vector2 Sprite::half_screen = 0.0f;
float Sprite::screen_mul = 1.0f;

Program*    Sprite::quad_prg_depth;
Program*    Sprite::quad_prg_no_depth;
VertexDecl* Sprite::vdecl;
DataBuffer* Sprite::buffer;

void Sprite::Load(JSONReader& loader, Sprite::Data* sprite, const char* name)
{
	if (loader.EnterBlock(name))
	{
		loader.Read("width", sprite->width);
		loader.Read("height", sprite->height);
		loader.Read("type", (int&)sprite->type);
		loader.Read("color", sprite->color);

		loader.Read("texName", sprite->tex_name);
		loader.Read("texMode", (int&)sprite->mode);
		loader.Read("texFilter", (int&)sprite->filter);

		if (sprite->type == Frames) loader.Read("frame_time", sprite->frame_time);

		int count = 1;
		loader.Read("count", count);
		sprite->rects.resize(count);

		for (int i = 0; i<count; i++)
		{
			Rect& rect = sprite->rects[i];

			loader.EnterBlock("Rect");

			loader.Read("pos", rect.pos);
			loader.Read("size", rect.size);
			loader.Read("uv", rect.uv);
			loader.Read("duv", rect.duv);
			loader.Read("offset", rect.offset);

			if (sprite->type == Frames)
			{
				loader.Read("frame_time", rect.frame_time);
			}

			loader.LeaveBlock();
		}

		loader.LeaveBlock();

		sprite->LoadTexture();
	}
}

void Sprite::Save(JSONWriter& saver, Sprite::Data* sprite, const char* name)
{
	saver.StartBlock(name);

	saver.Write("width", sprite->width);
	saver.Write("height", sprite->height);
	saver.Write("type", sprite->type);
	saver.Write("color", sprite->color);

	saver.Write("texName", sprite->tex_name.c_str());
	saver.Write("texMode", sprite->mode);
	saver.Write("texFilter", sprite->filter);

	if (sprite->type == Frames) saver.Write("frame_time", sprite->frame_time);

	int count = (int)sprite->rects.size();
	saver.Write("count", count);

	saver.StartArray("Rect");

	for (int i = 0; i<count; i++)
	{
		Rect& rect = sprite->rects[i];

		saver.StartBlock(nullptr);

		saver.Write("pos", rect.pos);
		saver.Write("size", rect.size);
		saver.Write("uv", rect.uv);
		saver.Write("duv", rect.duv);
		saver.Write("offset", rect.offset);

		if (sprite->type == Frames) saver.Write("frame_time", rect.frame_time);

		saver.FinishBlock();
	}

	saver.FinishArray();

	saver.FinishBlock();
}

void Sprite::Copy(Sprite::Data* src, Sprite::Data* dest)
{
	dest->width = src->width;
	dest->height = src->height;
	dest->type = src->type;
	dest->color = src->color;

	dest->mode = src->mode;
	dest->filter = src->filter;
	dest->tex_name = src->tex_name;

	dest->frame_time = src->frame_time;

	dest->texture = core.render.LoadTexture(dest->tex_name.c_str());

	int count = (int)src->rects.size();
	dest->rects.resize(count);

	for (int i = 0; i < count; i++)
	{
		dest->rects[i] = src->rects[i];
	}
}

void Sprite::Init()
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float2, VertexDecl::Position, 0 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(1, desc);

	int stride = sizeof(Vector2);
	buffer = core.render.GetDevice()->CreateBuffer(4, stride);

	Vector2* v = (Vector2*)buffer->Lock();

	v[0] = Vector2(0.0f, 1.0f);
	v[1] = Vector2(0.0f, 0.0f);
	v[2] = Vector2(1.0f, 1.0f);
	v[3] = Vector2(1.0f, 0.0f);

	buffer->Unlock();

	quad_prg_depth = core.render.GetProgram("QuadProgramDepth");
	quad_prg_no_depth = core.render.GetProgram("QuadProgramNoDepth");
}

void Sprite::Update()
{
	half_screen = Vector2(core.render.GetDevice()->GetWidth() * 0.5f, core.render.GetDevice()->GetHeight() * 0.5f);
	screen_mul = core.render.GetDevice()->GetHeight() / 1024.0f;
}

Vector2 Sprite::MoveToCamera(Vector2 pos, bool abs_units)
{
	if (abs_units)
	{
		pos *= screen_mul;
	}

	if (use_ed_cam)
	{
		pos -= ed_cam_pos;
		pos *= ed_cam_zoom;
	}
	else
	{
		pos -= cam_pos * screen_mul;
	}

	pos += half_screen;

	return pos;
}

float Sprite::ScaleToAbs(float size)
{
	return size / screen_mul;
}

Vector2 Sprite::MoveFromCamera(Vector2 pos, bool abs_units)
{
	pos -= half_screen;
	pos /= ed_cam_zoom;
	pos += ed_cam_pos;

	if (abs_units)
	{
		pos /= screen_mul;
	}

	return pos;
}

void Sprite::Draw(Texture* texture, Color clr, Matrix trans, Vector2 pos, Vector2 size, Vector2 uv, Vector2 duv, bool use_depth, bool flipped)
{
	core.render.GetDevice()->SetVertexBuffer(0, buffer);
	core.render.GetDevice()->SetVertexDecl(vdecl);

	Program* quad_prg = use_depth ? quad_prg_depth : quad_prg_no_depth;
	core.render.GetDevice()->SetProgram(quad_prg);

	Device::Viewport viewport;
	core.render.GetDevice()->GetViewport(viewport);

	Vector4 params[3];
	params[0] = Vector4(pos.x, pos.y, size.x, size.y);
	params[1] = Vector4(uv.x, uv.y, duv.x, duv.y);

	if (flipped)
	{
		params[1] = Vector4(uv.x + duv.x, uv.y, -duv.x, duv.y);
	}

	params[2] = Vector4((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight(), 0.5f, 0);

	quad_prg->SetVector(Shader::Type::Vertex, "desc", &params[0], 3);
	quad_prg->SetMatrix(Shader::Type::Vertex, "trans", &trans, 1);
	quad_prg->SetVector(Shader::Type::Pixel, "color", (Vector4*)&clr.r, 1);
	quad_prg->SetTexture(Shader::Type::Pixel, "diffuseMap", texture ? texture : core.render.GetWhiteTexture());

	core.render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
}

void Sprite::Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, FrameState* state, bool use_depth, bool ignore_camera)
{
	if (sprite->texture)
	{
		sprite->texture->SetAdress(sprite->mode);
		sprite->texture->SetFilters(sprite->filter, sprite->filter);
	}

	Matrix local_trans = trans->mat_global;

	local_trans.Pos().x *= screen_mul;
	local_trans.Pos().y *= screen_mul;

	Vector2 pos = trans->offset * trans->size * -1.0f;
	pos *= screen_mul;

	if (!ignore_camera)
	{
		if (use_ed_cam)
		{
			local_trans.Pos().x -= ed_cam_pos.x;
			local_trans.Pos().y -= ed_cam_pos.y;
		}
		else
		{
			local_trans.Pos().x -= cam_pos.x * screen_mul - half_screen.x;
			local_trans.Pos().y -= cam_pos.y * screen_mul - half_screen.y;
		}
	}

	Vector2 size = trans->size * screen_mul;

	if (!ignore_camera && use_ed_cam)
	{
		pos *= ed_cam_zoom;
		local_trans.Pos() *= ed_cam_zoom;

		local_trans.Pos().x -= -half_screen.x;
		local_trans.Pos().y -= -half_screen.y;

		size *= ed_cam_zoom;
	}

	Vector3 min_pos(10000000.0f);
	Vector3 max_pos(-10000000.0f);

	Vector3 tmp[] = { Vector3(pos.x, pos.y, 0), Vector3(pos.x + size.x, pos.y, 0), Vector3(pos.x + size.x, pos.y + size.y, 0), Vector3(pos.x, pos.y + size.y, 0) };
	
	for (int i = 0; i < 4; i++)
	{
		Vector3 temp = tmp[i] * local_trans;
		min_pos.Min(temp);
		max_pos.Max(temp);
	}

	if (max_pos.x < 0 || min_pos.x > core.render.GetDevice()->GetWidth() ||
		max_pos.y < 0 || min_pos.y > core.render.GetDevice()->GetHeight())
	{
		return;
	}

	clr *= sprite->color;

	if (!sprite->texture)
	{
		Draw(sprite->texture, clr, local_trans, pos, size, 0.0f, 1.0f, false);
	}
	else
	if (sprite->type == Image)
	{
		Rect& rect = sprite->rects[0];
		Draw(sprite->texture, clr, local_trans, pos, size, rect.uv, rect.duv, use_depth, state->horz_flipped);
	}
	else
	if (sprite->type == Frames)
	{
		Rect& start_rect = sprite->rects[0];
		Rect& rect = sprite->rects[state->cur_frame];

		Vector2 rect_scale = size / start_rect.size;
		Vector2 sz = rect_scale * rect.size;
		Vector2 dpos = rect_scale * rect.offset * screen_mul;

		if (state->horz_flipped)
		{
			dpos.x = -dpos.x;
		}

		dpos += Vector2((size.x - sz.x) * 0.5f, size.y - sz.y);

		Draw(sprite->texture, clr, local_trans, pos + dpos, sz, rect.uv, rect.duv, use_depth, state->horz_flipped);
	}
	else
	if (sprite->type == ThreeSlicesVert)
	{
		float y[] = { 0, sprite->rects[0].size.y, size.y - sprite->rects[2].size.y, size.y };

		for (int i = 0; i < 3; i++)
		{
			Rect& rect = sprite->rects[i];
			Draw(sprite->texture, clr, local_trans, pos + Vector2(0.0f, y[i]), Vector2(size.x, y[i+1] - y[i]), rect.uv, rect.duv, use_depth);
		}
	}
	else
	if (sprite->type == ThreeSlicesHorz)
	{
		float x[] = { 0, sprite->rects[0].size.x, size.x - sprite->rects[2].size.x, size.x };

		for (int i = 0; i < 3; i++)
		{
			Rect& rect = sprite->rects[i];
			Draw(sprite->texture, clr, local_trans, pos + Vector2(x[i], 0.0f), Vector2(x[i + 1] - x[i], size.y), rect.uv, rect.duv, use_depth);
		}
	}
	else
	if (sprite->type == NineSlices)
	{
		float x[] = { 0, sprite->rects[0].size.x, size.x - sprite->rects[2].size.x, size.x };
		float y[] = { 0, sprite->rects[0].size.y, size.y - sprite->rects[8].size.y, size.y };

		int index = 0;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				Rect& rect = sprite->rects[index];
				Draw(sprite->texture, clr, local_trans, pos + Vector2(x[j], y[i]), Vector2(x[j + 1] - x[j], y[i + 1] - y[i]), rect.uv, rect.duv, use_depth);
				index++;
			}
	}
}



