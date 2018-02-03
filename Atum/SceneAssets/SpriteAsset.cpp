
#include "SpriteAsset.h"
#include "Services/Render/Render.h"

#include "SpriteWindow.h"
#include "Editor/Gizmo.h"

void StartEditSprite(void* owner)
{
	SpriteAsset* sprite = (SpriteAsset*)owner;
	SpriteWindow::StartEdit(&sprite->sprite);
}

CLASSDECLDECL(SceneObject, SpriteAsset)

META_DATA_DESC(SpriteAsset)
FLOAT_PROP(SpriteAsset, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(SpriteAsset, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(SpriteAsset, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(SpriteAsset, trans.size.y, 100.0f, "Prop", "height")
CALLBACK_PROP(SpriteAsset, StartEditSprite, "Prop", "EditSprite")
META_DATA_DESC_END()

SpriteAsset::SpriteAsset() : SceneAsset()
{
}

SpriteAsset::~SpriteAsset()
{
}

void SpriteAsset::Init()
{
	quad.Init();

	//Tasks()->AddTask(100, this, (Object::Delegate)&Animation::Draw);
	RenderTasks()->AddTask(0, this, (Object::Delegate)&SpriteAsset::Draw);
}

void SpriteAsset::Load(JSONReader* loader)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Load(loader);
	Sprite::Load(loader, &sprite);
}

void SpriteAsset::Save(JSONWriter* saver)
{
	GetMetaData()->Prepare(this);
	GetMetaData()->Save(saver);
	Sprite::Save(saver, &sprite);
}

void SpriteAsset::Draw(float dt)
{
	trans.BuildLocalTrans();
	Sprite::Draw(&trans, COLOR_WHITE, &sprite, &quad);
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

void SpriteAsset::Play()
{
	SceneObject::Play();
}

void SpriteAsset::Stop()
{
	SceneObject::Stop();
}