
#include "AtumRunner.h"
#include "SceneObjects/2D/Sprite.h"
#include "Services/Scene/ExecuteLevels.h"

AtumRunner runner;

void AtumRunner::Init()
{
	if (inited)
	{
		return;
	}

	core.Init(nullptr, nullptr);

	core.scripts.Start();
	Sprite::use_ed_cam = false;

	core.render.AddExecutedLevelPool(1);

	renderTaskPool = core.render.AddTaskPool();
	renderTaskPool->AddTask(1, this, (Object::Delegate)&AtumRunner::Draw);

	core.scene_manager.LoadProject("project/project.pra");

	inited = true;
}

void AtumRunner::Resize(int width, int height)
{
	core.render.GetDevice()->SetVideoMode(width, height, nullptr);
}

void AtumRunner::Execute()
{
	core.CountDeltaTime();

	core.Update();
}

void AtumRunner::Draw(float dt)
{
	core.render.DebugPrintText(10.0f, ScreenCorner::RightTop, COLOR_GREEN, "%i", core.GetFPS());

	core.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

	core.render.ExecutePool(ExecuteLevels::Camera, dt);
	core.render.ExecutePool(ExecuteLevels::Prepare, dt);
	core.render.ExecutePool(ExecuteLevels::Geometry, dt);
	core.render.ExecutePool(ExecuteLevels::DebugGeometry, dt);
	core.render.ExecutePool(ExecuteLevels::Sprites, dt);
	core.render.ExecutePool(ExecuteLevels::Sprites + 1, dt);
	core.render.ExecutePool(ExecuteLevels::PostProcess, dt);
	core.render.ExecutePool(ExecuteLevels::GUI, dt);
	core.render.ExecutePool(ExecuteLevels::Debug, dt);
}