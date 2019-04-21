
#include "Trigger2D.h"

CLASSREG(SceneObject, Trigger2D, "Trigger2D")

META_DATA_DESC(Trigger2D)
BASE_SCENE_OBJ_PROP(Trigger2D)
FLOAT_PROP(Trigger2D, trans.pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(Trigger2D, trans.pos.y, 0.0f, "Prop", "y")
FLOAT_PROP(Trigger2D, trans.size.x, 100.0f, "Prop", "width")
FLOAT_PROP(Trigger2D, trans.size.y, 100.0f, "Prop", "height")
FLOAT_PROP(Trigger2D, trans.depth, 0.5f, "Prop", "Depth")
META_DATA_DESC_END()

void Trigger2D::Init()
{
#ifdef EDITOR
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + 2, this, (Object::Delegate)&Trigger2D::EditorDraw);
#endif

	script_callbacks.push_back(ScriptCallback("OnContactStart", "void", "%i%s%i"));
	script_callbacks.push_back(ScriptCallback("OnContactEnd", "void", "%i%s%i"));
}

bool Trigger2D::Play()
{
	float scale = 1.0f / 50.0f;

	Matrix body_trans;
	body_trans.Pos() = { trans.pos.x * scale, -trans.pos.y * scale, 0.0f };

	Matrix offset;

	body.object = this;
	body.body = PScene()->CreateBox({ trans.size.x * scale, trans.size.y * scale, 1.0f }, body_trans, offset, PhysObject::Trigger, 0);
	body.body->SetUserData(&body);

	return true;
}

void Trigger2D::Stop()
{
	if (body.body)
	{
		RELEASE(body.body);
	}
}

#ifdef EDITOR
void Trigger2D::EditorDraw(float dt)
{
	if (owner->Playing() || GetState() == State::Invisible)
	{
		return;
	}

	trans.BuildMatrices();
	Sprite::Data sprite;
	Sprite::FrameState state;
	Color color = COLOR_WHITE;
	color.a = 0.5f;
	trans.depth = 0.01f;
	Sprite::Draw(&trans, color, &sprite, &state, true, false);
}

bool Trigger2D::CheckSelection(Vector2 ms)
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	Vector2 pos = (trans.pos + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

	if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
		pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
	{
		return true;
	}

	return false;
}

void Trigger2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	Gizmo::inst->SetTrans2D(ed ? &trans : nullptr, Gizmo::trans_2d_move | Gizmo::trans_2d_scale);
}
#endif