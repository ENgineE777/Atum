
#include "Animation.h"
#include "Services/Render/Render.h"

CLASSDECLDECL(SceneObject, Animation)

META_DATA_DESC(Animation)
FLOAT_PROP(Animation, length, 1.0f, "Prop", "length")
META_DATA_DESC_END()

Animation::Animation() : SceneAsset()
{
}

Animation::~Animation()
{
}

void Animation::Init()
{
	quad.Init();
	tex = render.LoadTexture("Media/sprite_level16_2-hd.png");

	//Tasks()->AddTask(100, this, (Object::Delegate)&Animation::Draw);
	RenderTasks()->AddTask(0, this, (Object::Delegate)&Animation::Draw);
}

void Animation::Draw(float dt)
{
	quad.Draw(tex, Vector2(400 + length, 400 + length), Vector2(100, 100));
}

void Animation::Play()
{
	SceneObject::Play();
}

void Animation::Stop()
{
	SceneObject::Stop();
}