
#include "BoxPlayer.h"
#include "Services/Render/Render.h"
#include "../Animator.h"

CLASSREG(TrackPlayer, BoxPlayer, "Box")

META_DATA_DESC(BoxPlayer)
META_DATA_DESC_END()

BoxPlayer::BoxPlayer() : TrackPlayer()
{
}

BoxPlayer::~BoxPlayer()
{
}

void BoxPlayer::Init()
{
	AddTransformDataTrack(Transform, &trans, 1.0f, false, NULL)
	AddColorDataTrack(Color, &color, nullptr);
	AddFloatDataTrack(sizeX, &size.x, nullptr, 1.0f)
	AddFloatDataTrack(sizeY, &size.y, nullptr, 1.0f)
	AddFloatDataTrack(sizeZ, &size.z, nullptr, 1.0f)

	owner->RenderTasks(false)->AddTask(ExecuteLevels::Geometry, this, (Object::Delegate)&BoxPlayer::Draw);
}

void BoxPlayer::Draw(float dt)
{
	core.render.DebugBox(trans, color, size);
}