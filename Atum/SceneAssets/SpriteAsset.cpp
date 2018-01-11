
#include "SpriteAsset.h"
#include "Services/Render/Render.h"

CLASSDECLDECL(SceneObject, SpriteAsset)

META_DATA_DESC(SpriteAsset)
FLOAT_PROP(SpriteAsset, length, 1.0f, "Prop", "length")
META_DATA_DESC_END()

SpriteAsset::SpriteAsset() : SceneAsset()
{
}

SpriteAsset::~SpriteAsset()
{
}

void SpriteAsset::Init()
{
	quad.Init();
	tex = render.LoadTexture("Media/sprite_level16_2-hd.png");

	//Tasks()->AddTask(100, this, (Object::Delegate)&Animation::Draw);
	RenderTasks()->AddTask(0, this, (Object::Delegate)&SpriteAsset::Draw);
}

void SpriteAsset::Draw(float dt)
{
	quad.Draw(tex, Vector2(400 + length, 400 + length), Vector2(100, 100));
}

void SpriteAsset::Play()
{
	SceneObject::Play();
}

void SpriteAsset::Stop()
{
	SceneObject::Stop();
}