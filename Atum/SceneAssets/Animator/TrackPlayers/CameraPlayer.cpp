
#include "CameraPlayer.h"
#include "Services/Render/Render.h"

CLASSREG(TrackPlayer, CameraPlayer)

META_DATA_DESC(CameraPlayer)
META_DATA_DESC_END()

void CameraPlayer::Init()
{
	AddTransformDataTrack(Transform , &trans, 0, false, NULL)
	AddFloatDataTrack(Fov, &fov, NULL, 45.0f)
}

void CameraPlayer::SetTime(float time)
{
	TrackPlayer::SetTime(time);

	Vector tmp( 0, 1, 0 );
	Matrix view;
	view.BuildView(trans.Pos(), trans.Pos() + trans.Vz(), tmp);
	render.SetTransform(Render::View, view);

	Matrix proj;
	proj.BuildProjection(fov * RADIAN, (float)render.GetDevice()->GetHeight() / (float)render.GetDevice()->GetWidth(), 0.1f, 1500.0f );
	render.SetTransform(Render::Projection, proj);
}