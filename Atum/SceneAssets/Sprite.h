#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"

class Texture;

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
		Vector2 pos;
		Vector2 size;
		Vector2 uv;
		Vector2 duv;
		Vector2 offset = 0.0f;
		float frame_time = -1.0f;
	};
		
	struct Data
	{
		Texture* texture = nullptr;
		string   tex_name;
		Texture::TextureAddress mode = Texture::Wrap;
		Texture::FilterType filter = Texture::Linear;

		Type type = Image;
		int width = 0, height = 0;
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
	};

	static Program*        quad_prg;
	static VertexDecl*     vdecl;
	static GeometryBuffer* buffer;
	static bool inited;

	static bool use_ed_cam;
	static Vector2 cam_pos;
	static Vector2 ed_cam_pos;

	static void Load(JSONReader& loader, Sprite::Data* sprite);
	static void Save(JSONWriter& saver, Sprite::Data* sprite);
	static void Copy(Sprite::Data* src, Sprite::Data* dest);
	static void UpdateFrame(Sprite::Data* data, FrameState* state, float dt);
	static void Init();
	static void Draw(Texture* texture, Color clr, Matrix trans, Vector2 pos, Vector2 size, Vector2 uv, Vector2 duv, bool flipped = false);
	static void Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, FrameState* state);
};
