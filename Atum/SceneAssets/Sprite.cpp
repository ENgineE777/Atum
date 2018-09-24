
#include "Sprite.h"

void Sprite::Data::LoadTexture()
{
	RELEASE(texture);
	texture = render.LoadTexture(tex_name.c_str());
	width = texture ? texture->GetWidth() : 0;
	height = texture ? texture->GetHeight() : 0;
}

bool Sprite::use_ed_cam = true;
Vector2 Sprite::cam_pos = 0.0f;
Vector2 Sprite::ed_cam_pos = 0.0f;

Program*        Sprite::quad_prg_depth;
Program*        Sprite::quad_prg_no_depth;
VertexDecl*     Sprite::vdecl;
GeometryBuffer* Sprite::buffer;
Texture*        Sprite::white_tex = nullptr;

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

	dest->texture = render.LoadTexture(dest->tex_name.c_str());

	int count = (int)src->rects.size();
	dest->rects.resize(count);

	for (int i = 0; i < count; i++)
	{
		dest->rects[i] = src->rects[i];
	}
}

void Sprite::UpdateFrame(Sprite::Data* data, FrameState* state, float dt)
{
	if (data->type != Frames)
	{
		return;
	}

	if (state->cur_time < -0.5f)
	{
		state->cur_time = 0.0f;

		if (state->reversed)
		{
			state->cur_frame = (int)(data->rects.size() - 1);
		}
		else
		{
			state->cur_frame = 0;
		}
	}

	if (state->cur_frame >= data->rects.size())
	{
		state->cur_frame = 0;
	}

	state->cur_time += dt;
	float frame_tm = (data->rects[state->cur_frame].frame_time == -1) ? data->frame_time : data->rects[state->cur_frame].frame_time;

	while (state->cur_time > frame_tm)
	{
		state->cur_time -= frame_tm;

		if (state->reversed)
		{
			state->cur_frame--;

			if (state->cur_frame < 0)
			{
				if (state->looped)
				{
					state->cur_frame = (int)(data->rects.size() - 1);
				}
				else
				{
					state->finished = true;
					state->cur_frame++;
					state->cur_time = 0.0f;
					break;
				}
			}
		}
		else
		{
			state->cur_frame++;
			if (state->cur_frame >= data->rects.size())
			{
				if (state->looped)
				{
					state->cur_frame = 0;
				}
				else
				{
					state->finished = true;
					state->cur_frame--;
					state->cur_time = 0.0f;
					break;
				}
			}
		}

		frame_tm = (data->rects[state->cur_frame].frame_time == -1) ? data->frame_time : data->rects[state->cur_frame].frame_time;
	}

	//render.DebugPrintText(Vector2(10, 50), COLOR_YELLOW, "Stat state->cur_frame %i %4.3f", state->cur_frame, state->cur_time);
}

void Sprite::Init()
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float2, VertexDecl::Position, 0 } };
	vdecl = render.GetDevice()->CreateVertexDecl(1, desc);

	int stride = sizeof(Vector2);
	buffer = render.GetDevice()->CreateBuffer(4, stride);

	Vector2* v = (Vector2*)buffer->Lock();

	v[0] = Vector2(0.0f, 1.0f);
	v[1] = Vector2(0.0f, 0.0f);
	v[2] = Vector2(1.0f, 1.0f);
	v[3] = Vector2(1.0f, 0.0f);

	buffer->Unlock();

	quad_prg_depth = render.GetProgram("QuadProgramDepth");
	quad_prg_no_depth = render.GetProgram("QuadProgramNoDepth");

	white_tex = render.LoadTexture("settings/editor/white.png");
}

void Sprite::Draw(Texture* texture, Color clr, Matrix trans, Vector2 pos, Vector2 size, Vector2 uv, Vector2 duv, bool use_depth, bool flipped)
{
	render.GetDevice()->SetVertexBuffer(0, buffer);
	render.GetDevice()->SetVertexDecl(vdecl);

	Program* quad_prg = use_depth ? quad_prg_depth : quad_prg_no_depth;
	render.GetDevice()->SetProgram(quad_prg);

	Device::Viewport viewport;
	render.GetDevice()->GetViewport(viewport);

	Vector4 params[3];
	params[0] = Vector4(pos.x, pos.y, size.x, size.y);
	params[1] = Vector4(uv.x, uv.y, duv.x, duv.y);

	if (flipped)
	{
		params[1] = Vector4(uv.x + duv.x, uv.y, -duv.x, duv.y);
	}

	params[2] = Vector4((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight(), 0.5f, 0);

	quad_prg->SetVector(Program::Vertex, "desc", &params[0], 3);
	quad_prg->SetMatrix(Program::Vertex, "trans", &trans, 1);
	quad_prg->SetVector(Program::Pixel, "color", (Vector4*)&clr.r, 1);
	quad_prg->SetTexture(Program::Pixel, "diffuseMap", texture);

	render.GetDevice()->Draw(Device::TriangleStrip, 0, 2);
}

void Sprite::Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, FrameState* state, bool use_depth, bool ignore_camera)
{
	if (sprite->texture)
	{
		sprite->texture->SetAdress(sprite->mode);
		sprite->texture->SetFilters(sprite->filter, sprite->filter);
	}

	Matrix local_trans = trans->mat_global;

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	local_trans.Pos().x *= scale;
	local_trans.Pos().y *= scale;

	Vector2 pos = trans->offset * trans->size * -1.0f;
	pos *= scale;

	if (!ignore_camera)
	{
		if (use_ed_cam)
		{
			local_trans.Pos().x -= ed_cam_pos.x - render.GetDevice()->GetWidth() * 0.5f;
			local_trans.Pos().y -= ed_cam_pos.y - 512 * scale;
		}
		else
		{
			local_trans.Pos().x -= cam_pos.x * scale - render.GetDevice()->GetWidth() * 0.5f;
			local_trans.Pos().y -= cam_pos.y * scale - 512 * scale;
		}
	}

	Vector2 size = trans->size * scale;

	if (local_trans.Pos().x + pos.x + size.x < 0 || local_trans.Pos().x + pos.x > render.GetDevice()->GetWidth() ||
		local_trans.Pos().y + pos.y + size.y < 0 || local_trans.Pos().y + pos.y > render.GetDevice()->GetHeight())
	{
		return;
	}

	clr *= sprite->color;

	if (!sprite->texture)
	{
		Draw(white_tex, clr, local_trans, pos, size, 0.0f, 1.0f, false);
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
		Vector2 dpos = rect_scale * rect.offset * scale;

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



