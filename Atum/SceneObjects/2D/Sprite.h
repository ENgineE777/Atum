#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"

class Texture;

#ifndef DOXYGEN_SKIP

class Sprite
{
public:

	enum Type
	{
		Image,
		ThreeSlicesVert,
		ThreeSlicesHorz,
		NineSlices,
		Frames
	};

	struct Rect
	{
		Vector2 pos = 0.0f;
		Vector2 size = 0.0f;
		Vector2 uv = 0.0f;
		Vector2 duv = 1.0f;
		Vector2 offset = 0.0f;
		float frame_time = -1.0f;
	};

	struct Data
	{
		Texture* texture = nullptr;
		string   tex_name;
		Color    color = COLOR_WHITE;
		Texture::TextureAddress mode = Texture::Wrap;
		Texture::FilterType filter = Texture::Linear;

		Type type = Image;
		int width = 32, height = 32;
		int tileType = 0;
		std::vector<Rect> rects;

		float frame_time = 0.25f;

		void LoadTexture();
	};

	struct FrameState
	{
		bool  looped = true;
		bool  finished = false;
		bool  reversed = false;
		bool  horz_flipped = false;
		int   cur_frame = 0;
		float cur_time = -1.0f;
		float frame_start_time = 0.0f;
	};

	static Program*    quad_prg_depth;
	static Program*    quad_prg_no_depth;
	static VertexDecl* vdecl;
	static DataBuffer* buffer;

	static bool use_ed_cam;
	static Vector2 cam_pos;
	static Vector2 ed_cam_pos;
	static float ed_cam_zoom;
	static Vector2 half_screen;
	static float screen_mul;
	static float inv_screen_mul;

	static void Load(JSONReader& loader, Sprite::Data* sprite, const char* name);
	static void Save(JSONWriter& saver, Sprite::Data* sprite, const char* name);
	static void Copy(Sprite::Data* src, Sprite::Data* dest);

	template<typename Func>
	static void UpdateFrame(Sprite::Data* data, FrameState* state, float dt, Func callback)
	{
		if (data->type != Frames)
		{
			return;
		}

		if (state->cur_time < -0.5f || state->cur_frame >= data->rects.size())
		{
			state->cur_time = 0.0f;
			state->finished = false;
			state->frame_start_time = 0.0f;

			if (state->reversed)
			{
				state->cur_frame = (int)(data->rects.size() - 1);
			}
			else
			{
				state->cur_frame = 0;
			}
		}

		if (state->finished)
		{
			return;
		}

		float frame_tm = (data->rects[state->cur_frame].frame_time < 0.01f) ? data->frame_time : data->rects[state->cur_frame].frame_time;

		while (state->cur_time + dt > state->frame_start_time + frame_tm)
		{
			state->frame_start_time += frame_tm;

			dt -= (state->frame_start_time - state->cur_time);

			callback(state->cur_time, state->frame_start_time);

			state->cur_time = state->frame_start_time;

			if (state->reversed)
			{
				state->cur_frame--;

				if (state->cur_frame < 0)
				{
					if (state->looped)
					{
						state->cur_frame = (int)(data->rects.size() - 1);
						state->cur_time = 0.0f;
						state->frame_start_time = 0.0f;
					}
					else
					{
						state->finished = true;
						state->cur_frame++;
						return;
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
						state->cur_time = 0.0f;
						state->frame_start_time = 0.0f;
					}
					else
					{
						state->finished = true;
						state->cur_frame--;
						return;
					}
				}
			}

			frame_tm = (data->rects[state->cur_frame].frame_time < 0.01f) ? data->frame_time : data->rects[state->cur_frame].frame_time;
		}

		callback(state->cur_time, state->cur_time + dt);

		state->cur_time += dt;
	}

	static void UpdateFrame(Sprite::Data* data, FrameState* state, float dt)
	{
		UpdateFrame(data, state, dt, [](float from, float to) {});
	}

	static void Init();
	static void Update();
	static Vector2 MoveToCamera(Vector2 pos, bool abs_units = true);
	static float ScaleToAbs(float size);
	static Vector2 MoveFromCamera(Vector2 pos, bool abs_units = true);
	static void Draw(Texture* texture, Color clr, Matrix trans, Vector2 pos, Vector2 size, Vector2 uv, Vector2 duv, bool use_depth, bool flipped = false);
	static void Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, FrameState* state, bool use_depth, bool ignore_camera);
};
#endif