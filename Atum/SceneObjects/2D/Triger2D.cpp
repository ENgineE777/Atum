
#include "Triger2D.h"

CLASSREG(SceneObject, Triger2D, "Triger2D")

META_DATA_DESC(Triger2D)
BASE_SCENE_OBJ_PROP(Triger2D)
FLOAT_PROP(Triger2D, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(Triger2D, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(Triger2D, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(Triger2D, trans.size.y, 100.0f, "Prop", "height")
FLOAT_PROP(Triger2D, trans.depth, 0.5f, "Prop", "Depth")
META_DATA_DESC_END()

void Triger2D::Init()
{
#ifdef EDITOR
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites, this, (Object::Delegate)&Triger2D::EditorDraw);
#endif

	script_callbacks.push_back(ScriptCallback("OnContact", "int", "%i%s%i"));
}

bool Triger2D::Play()
{
	b2BodyDef bodyDef;

	float scale = 1.0f / 50.0f;

	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(trans.pos.x * scale, trans.pos.y * scale);

	b2PolygonShape box;
	box.SetAsBox(trans.size.x * scale * 0.5f, trans.size.y * scale * 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	fixtureDef.shape = &box;

	body.object = this;
	body.body = PScene2D()->CreateBody(&bodyDef);
	body.body->CreateFixture(&fixtureDef);
	body.body->SetUserData(&body);

	return true;
}

void Triger2D::Stop()
{
	if (body.body)
	{
		PScene2D()->DestroyBody(body.body);
	}
}

#ifdef EDITOR
void Triger2D::EditorDraw(float dt)
{
	if (owner->Playing())
	{
		return;
	}

	trans.BuildMatrices();
	Sprite::Data sprite;
	Sprite::FrameState state;
	Color color = COLOR_WHITE;
	color.a = 0.5f;
	Sprite::Draw(&trans, color, &sprite, &state, true, false);
}

bool Triger2D::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	Vector2 pos = (trans.pos + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

	if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
		pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
	{
		return true;
	}

	return false;
}

void Triger2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	Gizmo::inst->SetTrans2D(ed ? &trans : nullptr, Gizmo::trans_2d_move | Gizmo::trans_2d_scale);
}
#endif