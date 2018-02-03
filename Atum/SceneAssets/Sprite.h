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
		NineSlices
	};

	struct Rect
	{
		Vector2 pos;
		Vector2 size;
		Vector2 uv;
		Vector2 duv;
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
		Rect rects[9];

		void Load();
	};

	inline static int GetRectCount(Type tp)
	{
		if (tp == ThreeSlicesVert || tp == ThreeSlicesHorz)
		{
			return 3;
		}
		else
		if (tp == NineSlices)
		{
			return 9;
		}
		return 1;
	}

	static void Load(JSONReader* loader, Sprite::Data* sprite);
	static void Save(JSONWriter* saver, Sprite::Data* sprite);
	static void Copy(Sprite::Data* src, Sprite::Data* dest);
	static void Draw(Transform2D* trans, Color clr, Sprite::Data* sprite, Quad* quad);
};
