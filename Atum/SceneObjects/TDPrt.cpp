
#include "TDPrt.h"
#include "Services/Render/Render.h"
#include "Programs.h"

CLASSDECLDECL(SceneObject, TDPrt)

META_DATA_DESC(TDPrt)
COLOR_PROP(TDPrt, color, COLOR_YELLOW, "Geometry", "color")
FLOAT_PROP(TDPrt, sizeX, 1.0f, "Geometry", "SizeX")
FLOAT_PROP(TDPrt, sizeY, 1.0f, "Geometry", "SizeY")
FLOAT_PROP(TDPrt, sizeZ, 1.0f, "Geometry", "SizeZ")
BOOL_PROP(TDPrt, isStatic, false, "Physics", "Is Static")
META_DATA_DESC_END()

TDPrt::TDPrt() : SceneObject()
{
}

TDPrt::~TDPrt()
{
}

void TDPrt::Init()
{
	Programs::Init();

	quad.Init();

	RenderTasks()->AddTask(0, this, (Object::Delegate)&TDPrt::Draw);
	owner->AddToGroup(this, "TDPrt");

	map_tex = render.LoadTexture("Media/sprite_level16_2-hd.png");
	tower_tex = render.LoadTexture("Media/tower.png");

	enemy_tex[0] = render.LoadTexture("Media/enemy_anim1.png");
	enemy_tex[1] = render.LoadTexture("Media/enemy_anim2.png");
	enemy_tex[2] = render.LoadTexture("Media/enemy_anim3.png");
}

void TDPrt::Draw(float dt)
{
	quad.Draw(map_tex, Vector2(0.0f, 0.0f), Vector2(800.0f, 800.0f));
	quad.Draw(tower_tex, Vector2(127.0f, 710.0f), Vector2(95.0f, 80.0f));
	quad.Draw(tower_tex, Vector2(407.0f, 710.0f), Vector2(95.0f, 80.0f));

	enemy.anim += dt;
	if (enemy.anim > 0.45f)
	{
		enemy.anim -= 0.45f;
	}

	quad.Draw(enemy_tex[(int)(enemy.anim / 0.15f)], Vector2(407.0f, 310.0f), Vector2(43.0f, 43.0f));
}

void TDPrt::Play()
{
	SceneObject::Play();
}

void TDPrt::Stop()
{
	SceneObject::Stop();
}