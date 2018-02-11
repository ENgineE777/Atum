#pragma once

#include "Support/Support.h"
#include "Services/Render/Render.h"
#include "../SceneObjects/quad.h"

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

		float cur_time = 0.0f;
		float frame_time = 0.25f;
		int   cur_frame = 0;

		void Load();
		void Update(float dt);
	};

	static void Load(JSONReader* loader, Sprite::Data* sprite);
	static void Save(JSONWriter* saver, Sprite::Data* sprite);
	static void Copy(Sprite::Data* src, Sprite::Data* dest);
	static void Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, Quad* quad);
};
