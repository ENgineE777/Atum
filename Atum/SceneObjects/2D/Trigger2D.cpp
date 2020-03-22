
#include "Trigger2D.h"

CLASSREG(SceneObject, Trigger2D, "Trigger2D")

META_DATA_DESC(Trigger2D)
BASE_SCENE_OBJ_PROP(Trigger2D)
FLOAT_PROP(Trigger2D, trans.pos.x, 0.0f, "Prop", "x", "X coordinate of position of a trigger")
FLOAT_PROP(Trigger2D, trans.pos.y, 0.0f, "Prop", "y", "Y coordinate of position of a trigger")
FLOAT_PROP(Trigger2D, trans.size.x, 100.0f, "Prop", "width", "Width of a trigger")
FLOAT_PROP(Trigger2D, trans.size.y, 100.0f, "Prop", "height", "Height of a trigger")
INT_PROP(Trigger2D, collision_group, 1, "Phys2D", "collision_group", "Group of a body")
META_DATA_DESC_END()

void Trigger2D::Init()
{
#ifdef EDITOR
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + 2, this, (Object::Delegate)&Trigger2D::EditorDraw);
#endif

	script_callbacks.push_back(ScriptCallback(GetScene(), "OnContactStart", "void", "%i%s%i"));
	script_callbacks.push_back(ScriptCallback(GetScene(), "OnContactEnd", "void", "%i%s%i"));
}

void Trigger2D::SetState(State state)
{
	body.body->SetActive(state == State::Active);

	SceneObject::SetState(state);
}

void Trigger2D::BindClassToScript()
{
	const char* brief = "Representation of a triger in 2D space\n"
	"\n"
	"Whenever a physical object starts intersection with trigger script callback OnContactStart is called.\n"
	"Script callback OnContactEnd called after object leaves area of a trigger.\n";

	BIND_TYPE_TO_SCRIPT(Trigger2D, brief)
}

bool Trigger2D::Play()
{
	float scale = 1.0f / 50.0f;

	Matrix body_trans;
	body_trans.Pos() = { trans.pos.x * scale, -trans.pos.y * scale, 0.0f };

	if (parent_trans)
	{
		body_trans.Pos() = { (parent_trans->pos.x + trans.pos.x) * scale, -(parent_trans->pos.y + trans.pos.y) * scale, 0.0f };
	}

	Matrix offset;

	body.object = this;
	body.body = PScene()->CreateBox({ trans.size.x * scale, trans.size.y * scale, 1.0f }, body_trans, offset, PhysObject::Trigger, collision_group);
	body.body->SetUserData(&body);

	return true;
}

void Trigger2D::Release()
{
	RELEASE(body.body);

	SceneObject::Release();
}

#ifdef EDITOR
void Trigger2D::Set2DPos(Vector2 pos)
{
	trans.pos = pos;
}

void Trigger2D::EditorDraw(float dt)
{
	if (GetScene()->Playing() || GetState() == State::Invisible)
	{
		return;
	}

	if (parent_trans)
	{
		parent_trans->depth = 0.0f;
		parent_trans->BuildMatrices();
		trans.mat_parent = parent_trans->mat_global;
	}

	trans.BuildMatrices();
	Sprite::Data sprite;
	Sprite::FrameState state;
	Color color = COLOR_WHITE;
	color.a = 0.5f;
	trans.depth = 0.01f;
	Sprite::Draw(&trans, color, &sprite, &state, true, false);
}

bool Trigger2D::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	ms = Sprite::MoveFromCamera(ms);

	Vector2 pos = (trans.pos - trans.offset * trans.size);

	if (pos.x < ms.x && ms.x < pos.x + trans.size.x &&
		pos.y < ms.y && ms.y < pos.y + trans.size.y)
	{
		return true;
	}

	return false;
}

void Trigger2D::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	Gizmo::inst->SetTrans2D(Gizmo::Transform2D(trans), Gizmo::trans_2d_move | Gizmo::trans_2d_scale);

	if (!ed)
	{
		Gizmo::inst->Disable();
	}
}
#endif