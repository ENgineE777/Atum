#include "Scene2.h"

void Scene2::Init()
{
	quad.Init();

	//plane.Init(5.0f, "Media//grass.tga");

	correction.Init();

	//freeCamera.Init();

	model.LoadModelMS3D("Media//odst.ms3d");

	terrain.Init(Vector2(0.5f, 0.1f), "Media//terrain_c.tga", "Media//terrain_h.tga");

	hoverTank.Init(&terrain);

	num_drawers = 15;
	model_drawers = new Model::Drawer[num_drawers];

	/*for (int i = 0; i < num_drawers; i++)
	{
		model_drawers[i].Init(&model);

		Vector2 pos(rnd_range(-7.5f, 7.5f), rnd_range(-7.5f, 7.5f));
		model_drawers[i].SetPosition(pos);

		Vector4 color(rnd_range(0.5f, 1.0f), rnd_range(0.5f, 1.0f), rnd_range(0.5f, 1.0f));
		model_drawers[i].SetColor(color);
	}*/

	logo_tex = render.LoadTexture("Media//52664-200.tga");
	sky_tex = render.LoadTexture("Media//sky.tga");

	shadow = render.GetDevice()->CreateTexture(1024, 1024, Texture::FMT_D16, 1, true, Texture::Tex2D);

	scene_rt = render.GetDevice()->CreateTexture(800, 600, Texture::FMT_A8R8G8B8, 1, true, Texture::Tex2D);
	scene_depth = render.GetDevice()->CreateTexture(800, 600, Texture::FMT_D16, 1, true, Texture::Tex2D);

	cam_angle = 0;
	light_proj.BuildProjection(45.0f * RADIAN, 1.0f, 1.0f, 100.0f);

	//render.AddDelegate("prepare", this, (Object::Delegate)&Scene2::RenderPrepare, 0);
	//render.AddDelegate("shadows", this, (Object::Delegate)&Scene::RenderToShadow, 0);
	//render.AddDelegate("main", this, (Object::Delegate)&Scene2::Render, 0);
}

void Scene2::RenderPrepare(float dt)
{
	cam_angle -= 60 * RADIAN * dt * 0.5f;

	float r = 35;
	light_view.BuildView(Vector(cosf(-cam_angle) * r, r, sinf(-cam_angle) * r), 0.0f, Vector(0, 1, 0));

	Matrix light_view_proj = light_view * light_proj;

	Program::SetMatrix("sh_trans", &light_view_proj);
	Program::SetTexture("shdMap", shadow);
}

void Scene2::RenderToShadow(float dt)
{
	render.GetDevice()->SetRenderTarget(0, NULL);
	render.GetDevice()->SetDepth(shadow);

	render.GetDevice()->Clear(true, COLOR_WHITE, true, 1.0f);

	render.GetDevice()->SetupSlopeZBias(true, 0.5f, 0.5f);

	render.SetTransform(Render::View, light_view);
	render.SetTransform(Render::Projection, light_proj);

	//render.ExecuteDelgateList("toshadow", dt);

	render.GetDevice()->SetupSlopeZBias(false, 0.001f, 0.001f);

	render.GetDevice()->RestoreRenderTarget();
}

void Scene2::RenderScene(const char* phase, float dt)
{
	for (int i = 0; i < 20; i++)
	{
		int pos = i - 10;

		render.DebugLine(Vector(pos, 0, 0), COLOR_WHITE, Vector(pos, 0, 0), COLOR_WHITE);
		render.DebugLine(Vector(0, 0, pos), COLOR_WHITE, Vector(0, 0, pos), COLOR_WHITE);
	}

	render.GetDevice()->SetRenderTarget(0, scene_rt);
	render.GetDevice()->SetDepth(scene_depth);

	render.GetDevice()->Clear(false, COLOR_BLUE, true, 1.0f);

	quad.Draw(sky_tex, Vector2(0.0f, 0.0f), Vector2(800.0f, 600.0f));

	//render.ExecuteDelgateList(phase, dt);

	//render.ExecuteDelgateList("debug", dt);

	render.GetDevice()->RestoreRenderTarget();
}

void Scene2::RenderCorrection(float dt)
{
	render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

	correction.Draw(scene_rt);

	quad.Draw(logo_tex, Vector2(380.0f, 280.0f), Vector2(40.0f));
}

void Scene2::Render(float dt)
{
	RenderScene("geometry", dt);

	RenderCorrection(dt);

	render.GetDevice()->Present();
}
