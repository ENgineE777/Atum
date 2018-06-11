
#include "SpriteObject.h"
#include "Services/Render/Render.h"
#include "Editor/Gizmo.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpriteObject)

META_DATA_DESC(SpriteObject)
FLOAT_PROP(SpriteObject, trans.pos.x, 100.0f, "Geometry", "PosX")
FLOAT_PROP(SpriteObject, trans.pos.y, 100.0f, "Geometry", "PosY")
FLOAT_PROP(SpriteObject, trans.depth, 0.5f, "Geometry", "Depth")
STRING_PROP(SpriteObject, asset_name, "", "Prop", "Asset")
BOOL_PROP(SpriteObject, state.reversed, false, "Node", "Reversed")
META_DATA_DESC_END()

SpriteObject::SpriteObject() : SceneObject()
{
}

SpriteObject::~SpriteObject()
{
}

void SpriteObject::Init()
{
	RenderTasks()->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteObject::Draw);
}

void SpriteObject::ApplyProperties()
{
	state.cur_time = -1.0f;
	asset = (SpriteAsset*)owner->FindInGroup("SpriteAsset", asset_name.c_str());
}

void SpriteObject::Draw(float dt)
{
	if (!asset)
	{
		return;
	}

	trans.size = asset->trans.size;
	trans.BuildMatrices();
	Sprite::UpdateFrame(&asset->sprite, &state, dt);
	Sprite::Draw(&trans, COLOR_WHITE, &asset->sprite, &state, true);
}

#ifdef EDITOR
void SpriteObject::SetEditMode(bool ed)
{
	if (ed)
	{
		Gizmo::inst->trans2D = &trans;
	}
	else
	{
		Gizmo::inst->trans2D = nullptr;
	}
}
#endif