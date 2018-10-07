
#include "SpritePlayer.h"
#include "Services/Render/Render.h"
#include "../Animator.h"
#include "SceneObjects/2D/Sprite.h"

CLASSREG(TrackPlayer, SpritePlayer, "Sprite")

META_DATA_DESC(SpritePlayer)
META_DATA_DESC_END()

SpritePlayer::SpritePlayer() : TrackPlayer()
{
}

SpritePlayer::~SpritePlayer()
{
}

void SpritePlayer::Init()
{
	AddTransform2DDataTrack(Transform, &trans, NULL)

	tex = render.LoadTexture("Media/sprite_level16_2-hd.png");

	owner->RenderTasks(false)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&SpritePlayer::Draw);
}

void SpritePlayer::Draw(float dt)
{
	trans.BuildMatrices();
	Sprite::Draw(tex, COLOR_WHITE, trans.mat_global, trans.offset * trans.size * -1.0f, trans.size, 0.0f, 1.0f, true);
}