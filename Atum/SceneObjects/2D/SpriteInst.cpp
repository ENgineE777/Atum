
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "Phys2DComp.h"
#include "Track2DComp.h"

META_DATA_DESC(SpriteInst::Instance)
	FLOAT_PROP(SpriteInst::Instance, pos.x, 0.0f, "Prop", "x")
	FLOAT_PROP(SpriteInst::Instance, pos.y, 0.0f, "Prop", "y")
	INT_PROP(SpriteInst::Instance, index, -1, "Prop", "index")
	INT_PROP(SpriteInst::Instance, visible, 1, "Prop", "visible")
	COLOR_PROP(SpriteInst::Instance, color, COLOR_WHITE, "Prop", "color")
	FLOAT_PROP(SpriteInst::Instance, alpha, 0.0f, "Prop", "alpha")
META_DATA_DESC_END()

CLASSREG(SceneObject, SpriteInst, "Sprite")

META_DATA_DESC(SpriteInst)
	BASE_SCENE_OBJ_PROP(SpriteInst)
	FLOAT_PROP(SpriteInst, axis_scale, 1.0f, "Geometry", "axis_scale")
	FLOAT_PROP(SpriteInst, trans.depth, 0.5f, "Geometry", "Depth")
	INT_PROP(SpriteInst, draw_level, 0, "Geometry", "draw_level")
	ARRAY_PROP_INST(SpriteInst, instances, Instance, "Prop", "inst", SpriteInst, sel_inst)
META_DATA_DESC_END()

void SpriteInst::Instance::SetObject(asIScriptObject* set_object, vector<int>* set_mapping)
{
	object = set_object;
	mapping = set_mapping;

	SetPos(pos);
	SetFlipped(frame_state.horz_flipped);
	SetVisible(visible);
	SetAlpha(alpha);
	SetSizeX(size_x);
	SetAngle(angle);
}

void SpriteInst::Instance::SetPos(Vector2 set_pos)
{
	if (object)
	{
		int prop_x = mapping[0][0];
		int prop_y = mapping[0][1];

		if (prop_x != -1 && prop_y != -1)
		{
			*((float*)(object->GetAddressOfProperty(prop_x))) = set_pos.x;
			*((float*)(object->GetAddressOfProperty(prop_y))) = set_pos.y;
		}

		return;
	}

	pos = set_pos;
}

Vector2 SpriteInst::Instance::GetPos()
{
	if (object)
	{
		int prop_x = mapping[0][0];
		int prop_y = mapping[0][1];

		if (prop_x != -1 && prop_y != -1)
		{
			return Vector2(*((float*)object->GetAddressOfProperty(prop_x)), *((float*)object->GetAddressOfProperty(prop_y)));
		}
	}

	return pos;
}

void SpriteInst::Instance::SetFlipped(int set_horz_flipped)
{
	if (object)
	{
		int prop = mapping[0][2];

		if (prop != -1)
		{
			*((int*)object->GetAddressOfProperty(prop)) = set_horz_flipped;
		}

		return;
	}

	frame_state.horz_flipped = set_horz_flipped;
}

int SpriteInst::Instance::GetFlipped()
{
	if (object)
	{
		int prop = mapping[0][2];

		if (prop != -1)
		{
			return *((int*)object->GetAddressOfProperty(prop));
		}
	}

	return (frame_state.horz_flipped != 0);
}

void SpriteInst::Instance::SetVisible(int set_visible)
{
	if (object)
	{
		int prop = mapping[0][3];

		if (prop != -1)
		{
			*((int*)object->GetAddressOfProperty(prop)) = set_visible;
		}

		return;
	}

	visible = set_visible;
}

bool SpriteInst::Instance::IsVisible()
{
	if (object)
	{
		int prop = mapping[0][3];

		if (prop != -1)
		{
			return (*((int*)object->GetAddressOfProperty(prop)) != 0);
		}
	}

	return (visible != 0);
}

float SpriteInst::Instance::GetAlpha()
{
	if (object)
	{
		int prop = mapping[0][4];

		if (prop != -1)
		{
			return (*((float*)object->GetAddressOfProperty(prop)));
		}
	}

	return alpha;
}

void SpriteInst::Instance::SetAlpha(float set_alpha)
{
	if (object)
	{
		int prop = mapping[0][4];

		if (prop != -1)
		{
			*((float*)object->GetAddressOfProperty(prop)) = set_alpha;
			return;
		}
	}

	alpha = set_alpha;
}

float SpriteInst::Instance::GetSizeX()
{
	if (object)
	{
		int prop = mapping[0][5];

		if (prop != -1)
		{
			return (*((float*)object->GetAddressOfProperty(prop)));
		}
	}

	return size_x;
}

void  SpriteInst::Instance::SetSizeX(float set_size)
{
	if (object)
	{
		int prop = mapping[0][5];

		if (prop != -1)
		{
			*((float*)object->GetAddressOfProperty(prop)) = set_size;
			return;
		}
	}

	size_x = set_size;
}

float SpriteInst::Instance::GetAngle()
{
	if (object)
	{
		int prop = mapping[0][6];

		if (prop != -1)
		{
			return (*((float*)object->GetAddressOfProperty(prop)));
		}
	}

	return angle;
}

void SpriteInst::Instance::SetAngle(float set_angle)
{
	if (object)
	{
		int prop = mapping[0][6];

		if (prop != -1)
		{
			*((float*)object->GetAddressOfProperty(prop)) = set_angle;
			return;
		}
	}

	angle = set_angle;
}

void SpriteInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "int AddInstance(float x, float y, bool auto_delete)", WRAP_MFN(SpriteInst, AddInstance), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void RemoveInstance(int index)", WRAP_MFN(SpriteInst, RemoveInstance), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void ClearInstances()", WRAP_MFN(SpriteInst, ClearInstances), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void ApplyLinearImpulse(int index, float x, float y)", WRAP_MFN(SpriteInst, ApplyLinearImpulse), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void MoveTo(int index, float x, float y)", WRAP_MFN(SpriteInst, MoveTo), asCALL_GENERIC);
	scripts.engine->RegisterObjectMethod(script_class_name, "void SetActiveTrack(int index, bool set)", WRAP_MFN(SpriteInst, SetActiveTrack), asCALL_GENERIC);
}

void SpriteInst::MakeMapping(asIScriptObject* object)
{
	const char* names[] = { "x", "y", "horz_flipped", "visible", "alpha", "size_x", "angle" };
	int count = (sizeof(names) / sizeof(const char*));
	mapping.resize(count);

	for (int value_index = 0; value_index < count; value_index++)
	{
		mapping[value_index] = -1;

		for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
		{
			if (StringUtils::IsEqual(names[value_index], object->GetPropertyName(prop)))
			{
				mapping[value_index] = prop;
				break;
			}
		}
	}
}

bool SpriteInst::InjectIntoScript(const char* type, void* property)
{
	if (StringUtils::IsEqual(type, "array"))
	{
		array = (CScriptArray*)property;

		if (instances.size() > 0)
		{
			array->Resize((uint32_t)instances.size());
			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			MakeMapping(objects[0]);

			int index = 0;
			for (auto& inst : instances)
			{
				inst.SetObject(objects[index], &mapping);

				index++;
			}

			for (auto comp : components)
			{
				comp->InjectIntoScript(type, property);
			}
		}

		return true;
	}
	
	return SceneObject::InjectIntoScript(type, property);
}

void SpriteInst::Init()
{
	script_callbacks.push_back(ScriptCallback("OnContact", "int", "%i%s%i"));
}

void SpriteInst::ApplyProperties()
{
#ifdef EDITOR
	RenderTasks(false)->DelAllTasks(this);
#endif

	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + draw_level, this, (Object::Delegate)&SpriteInst::Draw);
}

bool SpriteInst::Play()
{
	trans.size = ((SpriteAsset*)asset)->trans.size;
	trans.offset = ((SpriteAsset*)asset)->trans.offset;

	return true;
}

void SpriteInst::Draw(float dt)
{ 
	if (state == Invisible)
	{
		return;
	}

	if (!asset)
	{
		return;
	}

	SpriteAsset* sprite_asset = (SpriteAsset*)asset;

#ifdef EDITOR
	if (edited)
	{
		if (rect_select)
		{
			if (controls.DebugKeyPressed("KEY_I"))
			{
				ClearRect();
			}
			else
			if (controls.DebugKeyPressed("KEY_O") || controls.DebugKeyPressed("KEY_P"))
			{
				FillRect();
			}
			else
			{
				if (Gizmo::inst->delta_move.Length() > 0.01f)
				{
					for (auto& index : sel_instances)
					{
						auto& inst = instances[index];
						inst.SetPos(inst.GetPos() + Gizmo::inst->delta_move);
					}
				}

				rect_p1 += Gizmo::inst->delta_move;
				rect_p2 += Gizmo::inst->delta_move;

				Gizmo::inst->delta_move = 0.0f;
			}
		}
		else
		{
			if (sel_inst != -1)
			{
				instances[sel_inst].SetPos(trans.pos);
			}

			if (controls.DebugKeyPressed("KEY_I") && sel_inst !=-1)
			{
				for (auto comp : components)
				{
					comp->InstDeleted(sel_inst);
				}

				instances.erase(sel_inst + instances.begin());
				sel_inst = -1;
				SetGizmo();
			}

			bool add_center = controls.DebugKeyPressed("KEY_O");
			bool add_after = controls.DebugKeyPressed("KEY_P");

			if (add_center || add_after)
			{
				Instance inst;

				if (sel_inst != -1 && add_after)
				{
					inst.SetPos(instances[sel_inst].GetPos() + 20.0f);
				}
				else
				{
					float scale = 1024.0f / render.GetDevice()->GetHeight();
					inst.SetPos({ Sprite::ed_cam_pos.x * scale, Sprite::ed_cam_pos.y * scale });
				}

				instances.push_back(inst);

				sel_inst = (int)instances.size() - 1;

				for (auto comp : components)
				{
					comp->InstAdded();
				}

				SetGizmo();
			}
		}
	}
#endif

	trans.offset = sprite_asset->trans.offset;
	trans.size = sprite_asset->trans.size;

	Vector2 pos = trans.pos;
	trans.pos *= axis_scale;

	Vector2 cam_pos = 0.0f;
	if (Sprite::use_ed_cam)
	{
		cam_pos = Sprite::ed_cam_pos;
		Sprite::ed_cam_pos *= axis_scale;
	}
	else
	{
		cam_pos = Sprite::cam_pos;
		Sprite::cam_pos *= axis_scale;
	}

	for (int i = 0; i<instances.size(); i++)
	{
		Instance& inst = instances[i];

		if (!inst.IsVisible())
		{
			continue;
		}

		if (state == Active)
		{
			Sprite::UpdateFrame(&sprite_asset->sprite, &inst.frame_state, dt);
		}

		if (inst.frame_state.finished && inst.auto_delete)
		{
			RemoveInstance(i);
			i--;
			continue;
		}

		if (inst.GetAlpha() < 0.01f)
		{
			continue;
		}

		trans.pos = inst.GetPos();
		float wgt = inst.GetSizeX();
		trans.size.x = wgt < -0.01f ? sprite_asset->trans.size.x : wgt;
		trans.rotation = inst.GetAngle();
		trans.BuildMatrices();

		inst.frame_state.horz_flipped = inst.GetFlipped();
		inst.color.a = inst.GetAlpha();

		Sprite::Draw(&trans, inst.color, &sprite_asset->sprite, &inst.frame_state, true, false);
	}

#ifdef EDITOR
	if (rect_select)
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		Vector2 delta = Sprite::ed_cam_pos - Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		for (auto& index : sel_instances)
		{
			auto& inst = instances[index];
			Vector2 pos = (inst.GetPos() - sprite_asset->trans.offset * sprite_asset->trans.size) * scale - delta;
			render.DebugRect2D(pos, pos + sprite_asset->trans.size * scale, COLOR_WHITE);
		}
	}

	if (edited)
	{
		if (sel_inst != -1)
		{
			trans.pos = instances[sel_inst].GetPos();
			trans.rotation = instances[sel_inst].GetAngle();
			trans.BuildMatrices();
		}
	}
#endif

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}

PhysObject* SpriteInst::HackGetBody(int index)
{
	for (auto comp : components)
	{
		if (!StringUtils::IsEqual(comp->class_name, "Phys2DCompInst"))
		{
			continue;
		}

		Phys2DCompInst* phys_comp = (Phys2DCompInst*)comp;

		phys_comp->UpdateInstances(0.0f);
		return phys_comp->bodies[index].body;
	}

	return nullptr;
}

int SpriteInst::AddInstance(float x, float y, bool auto_delete)
{
	instances.emplace_back();
	Instance& inst = instances.back();
	inst.SetPos({ x, y });
	inst.frame_state.looped = false;
	inst.auto_delete = auto_delete;

	int index = (int)instances.size() - 1;
	if (array)
	{
		array->Resize((uint32_t)instances.size());
		asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

		if (mapping.size() == 0)
		{
			MakeMapping(objects[0]);
		}

		inst.SetObject(objects[index], &mapping);
	}

	return index;
}

void SpriteInst::RemoveInstance(int index)
{
	instances.erase(instances.begin() + index);

	if (array)
	{
		array->RemoveAt(index);
	}
}

void SpriteInst::ClearInstances()
{
	instances.clear();

	if (array)
	{
		array->RemoveRange(0, array->GetSize());
	}
}

void SpriteInst::ApplyLinearImpulse(int index, float x, float y)
{
	PhysObject* body = HackGetBody(index);

	if (body)
	{
		body->AddForceAt({instances[index].GetPos().x / 50.0f, -instances[index].GetPos().x / 50.0f, 0.0f}, {x, y, 0.0f});
	}
}

void SpriteInst::MoveTo(int index, float x, float y)
{
	PhysObject* body = HackGetBody(index);

	if (body)
	{
		Matrix mat;
		mat.Pos() = { x / 50.0f, -y / 50.0f, 0.0f };
		body->SetTransform(mat);
	}
}

void SpriteInst::SetActiveTrack(int index, bool set)
{
	for (auto comp : components)
	{
		if (!StringUtils::IsEqual(comp->class_name, "Track2DComp"))
		{
			continue;
		}

		Track2DComp* track_comp = (Track2DComp*)comp;

		track_comp->tracks[index].active = set;
		break;
	}
}

#ifdef EDITOR
void SpriteInst::OnRectSelect(Vector2 p1, Vector2 p2)
{
	sel_inst = -1;
	rect_select = true;

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	Vector2 delta = Sprite::ed_cam_pos - Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

	rect_p1 = (p1 + delta) / scale;
	rect_p1 = Gizmo::inst->MakeAligned((p1 + delta) / scale);
	rect_p2 = (p2 + delta) / scale;
	rect_p2 = Gizmo::inst->MakeAligned((p2 + delta) / scale);

	sel_instances.clear();

	int index = 0;
	for (auto& inst : instances)
	{
		Vector2 pos = inst.GetPos();

		if (rect_p1.x - 0.01f < pos.x && pos.x < rect_p2.x + 0.01f &&
			rect_p1.y - 0.01f < pos.y && pos.y < rect_p2.y + 0.01f)
		{
			sel_instances.push_back(index);
		}

		index++;
	}

	multi_trans.offset = 0.0f;
	multi_trans.pos = rect_p1 - Vector2(trans.offset.x, trans.offset.y) * trans.size;
	multi_trans.size = rect_p2 + Vector2(1.0f - trans.offset.x, 1.0f - trans.offset.y) * trans.size - rect_p1;

	Gizmo::inst->SetTrans2D(&multi_trans, Gizmo::trans_2d_move);
}

void SpriteInst::ClearRect()
{
	sel_instances.clear();

	for (int index = (int)instances.size() - 1; index >= 0; index--)
	{
		auto& inst = instances[index];

		Vector2 pos = inst.GetPos();

		if (rect_p1.x - 0.01f < pos.x && pos.x < rect_p2.x + 0.01f &&
			rect_p1.y - 0.01f < pos.y && pos.y < rect_p2.y + 0.01f)
		{
			for (auto comp : components)
			{
				comp->InstDeleted(index);
			}

			instances.erase(index + instances.begin());
		}
	}
}

void SpriteInst::FillRect()
{
	ClearRect();

	float y = rect_p1.y;

	while (y < rect_p2.y + 0.01f)
	{
		float x = rect_p1.x;

		while (x < rect_p2.x + 0.01f)
		{
			Instance inst;

			inst.SetPos(Vector2(x, y));

			instances.push_back(inst);

			for (auto comp : components)
			{
				comp->InstAdded();
			}

			sel_instances.push_back((int)instances.size() - 1);

			x += trans.size.x;
		}

		y += trans.size.y;
	}
}

bool SpriteInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	rect_select = false;
	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Instance& inst = instances[i];

		Vector2 pos = (inst.GetPos() + trans.offset * trans.size * -1.0f) * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
			pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
		{
			sel_inst = i;

			if (edited)
			{
				SetGizmo();
			}

			return true;
		}
	}

	return false;
}

void SpriteInst::SetEditMode(bool ed)
{
	SceneObject::SetEditMode(ed);

	if (ed)
	{
		SetGizmo();
	}
}

void SpriteInst::SetGizmo()
{
	if (sel_inst != -1)
	{
		trans.pos = instances[sel_inst].GetPos();
	}

	Gizmo::inst->SetTrans2D(sel_inst != -1 ? &trans : nullptr, Gizmo::trans_2d_move);

	if (sel_inst != -1)
	{
		Gizmo::inst->SetTrans2DWidgets(instances[sel_inst].GetMetaData()->GetFloatEditBox("x"), instances[sel_inst].GetMetaData()->GetFloatEditBox("y"));
	}
}

int SpriteInst::GetInstCount()
{
	return (int)instances.size();
}
#endif
