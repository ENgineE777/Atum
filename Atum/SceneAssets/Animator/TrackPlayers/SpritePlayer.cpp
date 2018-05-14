
#include "SpritePlayer.h"
#include "Services/Render/Render.h"
#include "../Animator.h"
#include "SceneAssets/Sprite.h"
#include "SceneObjects/RenderLevels.h"

CLASSDECLDECL(SpritePlayer)

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

	owner->RenderTasks()->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpritePlayer::Draw);
}

void SpritePlayer::Draw(float dt)
{
	trans.BuildLocalTrans();
	Sprite::Draw(tex, COLOR_WHITE, trans.local_trans, trans.offset * trans.size * -1.0f, trans.size, 0.0f, 1.0f);
}