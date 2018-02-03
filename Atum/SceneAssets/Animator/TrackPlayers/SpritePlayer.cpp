
#include "SpritePlayer.h"
#include "Services/Render/Render.h"
#include "../Animator.h"

CLASSDECLDECL(TrackPlayer, SpritePlayer)

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

	quad.Init();
	tex = render.LoadTexture("Media/sprite_level16_2-hd.png");

	owner->RenderTasks()->AddTask(0, this, (Object::Delegate)&SpritePlayer::Draw);
}

void SpritePlayer::Draw(float dt)
{
	trans.BuildLocalTrans();
	quad.Draw(tex, trans.local_trans, trans.offset * trans.size * -1.0f, trans.size);
}