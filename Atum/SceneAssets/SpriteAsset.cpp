
#include "SpriteAsset.h"
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"
#include "SpriteWindow.h"

#ifdef EDITOR
void StartEditSprite(void* owner)
{
	SpriteAsset* sprite = (SpriteAsset*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}
#endif

CLASSREG(SceneAsset, SpriteAsset, "Sprite")

META_DATA_DESC(SpriteAsset)
BASE_SCENE_OBJ_NAME_PROP(SpriteAsset)
FLOAT_PROP(SpriteAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteAsset, trans.size.y, 100.0f, "Prop", "height")
#ifdef EDITOR
CALLBACK_PROP(SpriteAsset, StartEditSprite, "Prop", "EditSprite")
#endif
META_DATA_DESC_END()

Sprite::FrameState SpriteAsset::state;

SpriteAsset::SpriteAsset() : SceneAsset()
{
}

void SpriteAsset::Init()
{
	RenderTasks(true)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteAsset::Draw);

	owner->AddToGroup(this, "SpriteAsset");
}

void SpriteAsset::Load(JSONReader& loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);
	Sprite::Load(loader, &sprite);
}

void SpriteAsset::Save(JSONWriter& saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);
	Sprite::Save(saver, &sprite);
}

void SpriteAsset::Draw(float dt)
{
	trans.BuildMatrices();
	Sprite::UpdateFrame(&sprite, &state, dt);

#ifdef EDITOR
	if (Gizmo::inst->trans2D == &trans && SpriteWindow::instance && !SpriteWindow::instance->show_anim)
	{
		state.cur_frame = SpriteWindow::instance->cur_frame;
	}

	if (SpriteWindow::instance)
	{
		SpriteWindow::instance->CheckStateOfBorder();
	}
#endif

	Sprite::Draw(&trans, COLOR_WHITE, &sprite, &state, true, false);
}

#ifdef EDITOR
SceneObject* SpriteAsset::CreateInstance()
{
	SpriteInst* inst = (SpriteInst*)owner->AddObject("SpriteInst", false);
	inst->asset_uid = GetUID();
	inst->ApplyProperties();
	inst->SetName(GetName());

	instances.push_back(inst);

	return inst;
}

void SpriteAsset::Copy(SceneObject* src)
{
	Sprite::Copy(&((SpriteAsset*)src)->sprite, &sprite);
	SceneAsset::Copy(src);
}

void SpriteAsset::SetEditMode(bool ed)
{
	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
		SpriteWindow::StopEdit();
	}
}
#endif