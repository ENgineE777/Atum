
#include "SpriteGraphAsset.h"
#include "Services/Render/Render.h"

#include "SpriteWindow.h"
#include "Editor/Gizmo.h"

void StartEditGraphSprite(void* owner)
{
	SpriteGraphAsset* sprite = (SpriteGraphAsset*)owner;
	//SpriteWindow::StartEdit(&sprite->sprite);
}

CLASSDECLDECL(SceneObject, SpriteGraphAsset)

META_DATA_DESC(SpriteGraphAsset)
FLOAT_PROP(SpriteGraphAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteGraphAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteGraphAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteGraphAsset, trans.size.y, 100.0f, "Prop", "height")
CALLBACK_PROP(SpriteGraphAsset, StartEditGraphSprite, "Prop", "EditSprite")
META_DATA_DESC_END()

SpriteGraphAsset::SpriteGraphAsset() : SceneAsset()
{
}

SpriteGraphAsset::~SpriteGraphAsset()
{
}

void SpriteGraphAsset::Init()
{
	quad.Init();

	//Tasks()->AddTask(100, this, (Object::Delegate)&Animation::Draw);
	RenderTasks()->AddTask(0, this, (Object::Delegate)&SpriteGraphAsset::Draw);
}

void SpriteGraphAsset::Load(JSONReader* loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);
	Sprite::Load(loader, &sprite);
}

void SpriteGraphAsset::Save(JSONWriter* saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);
	Sprite::Save(saver, &sprite);
}

void SpriteGraphAsset::Draw(float dt)
{
	trans.BuildLocalTrans();
	sprite.Update(dt);
	Sprite::Draw(&trans, COLOR_WHITE, &sprite, &quad);
}

#ifdef EDITOR
void SpriteGraphAsset::SetEditMode(bool ed)
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