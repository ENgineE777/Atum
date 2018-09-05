
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpriteInst, "Sprite")

META_DATA_DESC(SpriteInst)
BASE_SCENE_OBJ_NAME_PROP(SpriteInst)
BASE_SCENE_OBJ_STATE_PROP(SpriteInst)
FLOAT_PROP(SpriteInst, axis_scale, 1.0f, "Geometry", "axis_scale")
FLOAT_PROP(SpriteInst, trans.depth, 0.5f, "Geometry", "Depth")
BOOL_PROP(SpriteInst, frame_state.horz_flipped, false, "Node", "horz_flipped")
META_DATA_DESC_END()

void SpriteInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteInst)
	scripts.engine->RegisterObjectMethod(script_class_name, "void AddInstance(float x, float y)", WRAP_MFN(SpriteInst, AddInstance), asCALL_GENERIC);
}

void SpriteInst::InjectIntoScript(const char* type, void* property)
{
	if (StringUtils::IsEqual(type, "array"))
	{
		array = (CScriptArray*)property;
		array->Resize((uint32_t)instances.size());
		asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

		int index = 0;
		for (auto& inst : instances)
		{
			*((float*)objects[index]->GetAddressOfProperty(0)) = inst.pos.x;
			*((float*)objects[index]->GetAddressOfProperty(1)) = inst.pos.y;
			*((int*)objects[index]->GetAddressOfProperty(2)) = inst.frame_state.horz_flipped ? 1 : 0;
			*((int*)objects[index]->GetAddressOfProperty(3)) = inst.visible;

			index++;
		}
	}
	else
	{
		SceneObject::InjectIntoScript(type, property);
	}
}

void SpriteInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteInst::Draw);
}

void SpriteInst::Load(JSONReader& loader)
{
	SceneObjectInst::Load(loader);

	frame_state.cur_time = -1.0f;
	asset = (SpriteAsset*)owner->FindByUID(asset_uid, 0, true);

	int count = 0;
	loader.Read("count", count);
	instances.resize(count);

	for (auto& inst : instances)
	{
		if (!loader.EnterBlock("inst")) break;

		loader.Read("pos", inst.pos);

		loader.LeaveBlock();
	}
}

void SpriteInst::Save(JSONWriter& saver)
{
	SceneObjectInst::Save(saver);

	saver.Write("count", (int)instances.size());

	saver.StartArray("inst");

	for (auto& inst : instances)
	{
		saver.StartBlock(nullptr);
		saver.Write("pos", inst.pos);
		saver.FinishBlock();
	}

	saver.FinishArray();
}

void SpriteInst::Play()
{
	float scale = 1.0f / 50.0f;

	trans.size = asset->trans.size;

	if (StringUtils::IsEqual(asset->GetName(), "Ground1_N") || StringUtils::IsEqual(asset->GetName(), "MovingBlock"))
	{
		for (auto& inst : instances)
		{
			b2BodyDef bodyDef;

			if (StringUtils::IsEqual(asset->GetName(), "MovingBlock"))
			{
				bodyDef.type = b2_kinematicBody;
			}

			bodyDef.position.Set(inst.pos.x * scale, inst.pos.y * scale);

			inst.body = PScene2D()->CreateBody(&bodyDef);

			b2PolygonShape box;
			box.SetAsBox(trans.size.x * 0.5f * scale, trans.size.y * 0.5f * scale);

			inst.body->CreateFixture(&box, 0.0f);
		}
	}
}

void SpriteInst::Stop()
{
	for (auto& inst : instances)
	{
		if (inst.body)
		{
			PScene2D()->DestroyBody(inst.body);
			inst.body = nullptr;
		}
	}
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

#ifdef EDITOR
	if (edited)
	{
		if (sel_inst != -1)
		{
			instances[sel_inst].pos = trans.pos;
		}

		if (controls.DebugKeyPressed("KEY_O") && sel_inst !=-1)
		{
			instances.erase(sel_inst + instances.begin());
			sel_inst = -1;
			SetGizmo();
		}

		if (controls.DebugKeyPressed("KEY_P"))
		{
			Instance inst;
			float scale = render.GetDevice()->GetHeight() / 1024.0f;
			inst.pos.x = ((Sprite::ed_cam_pos.x + render.GetDevice()->GetWidth()) * 0.5f) / scale;
			inst.pos.y = Sprite::ed_cam_pos.y / scale + 512.0f;
			instances.push_back(inst);

			sel_inst = (int)instances.size() - 1;

			SetGizmo();
		}
	}
#endif

	trans.size = asset->trans.size;

	if (state == Active)
	{
		Sprite::UpdateFrame(&asset->sprite, &frame_state, dt);
	}

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

		if (array)
		{
			asIScriptObject** objects = (asIScriptObject**)array->GetBuffer();

			inst.pos.x = *((float*)objects[i]->GetAddressOfProperty(0));
			inst.pos.y = *((float*)objects[i]->GetAddressOfProperty(1));
			inst.frame_state.horz_flipped = (*((int*)objects[i]->GetAddressOfProperty(2)) > 0);
			inst.visible = *((int*)objects[i]->GetAddressOfProperty(3));
		}

		if (inst.visible == 0)
		{
			continue;
		}

		if (StringUtils::IsEqual(asset->GetName(), "MovingBlock") && inst.body)
		{
			inst.body->SetTransform({ inst.pos.x / 50.0f, inst.pos.y / 50.0f }, 0.0f);
		}

		if (state == Active)
		{
			Sprite::UpdateFrame(&asset->sprite, &inst.frame_state, dt);
		}

		if (inst.frame_state.finished)
		{
			instances.erase(instances.begin() + i);
			i--;
			continue;
		}

		trans.pos = inst.pos;

		trans.BuildMatrices();

		Sprite::Draw(&trans, COLOR_WHITE, &asset->sprite, &inst.frame_state, true, false);
	}

	trans.pos = pos;

	if (Sprite::use_ed_cam)
	{
		Sprite::ed_cam_pos = cam_pos;
	}
	else
	{
		Sprite::cam_pos = cam_pos;
	}
}

void SpriteInst::AddInstance(float x, float y)
{
	Instance inst;
	inst.pos.x = x;
	inst.pos.y = y;
	inst.frame_state.looped = false;
	instances.push_back(inst);
}

#ifdef EDITOR
bool SpriteInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_inst = -1;
	for (int i = 0; i < instances.size(); i++)
	{
		Instance& inst = instances[i];

		Vector2 pos = inst.pos + trans.offset * trans.size * -1.0f;
		pos *= scale;
		pos.x -= Sprite::ed_cam_pos.x;
		pos.y -= Sprite::ed_cam_pos.y;

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

	Gizmo::inst->enabled = ed;

	if (ed)
	{
		SetGizmo();
	}
}

void SpriteInst::SetGizmo()
{
	Gizmo::inst->trans2D = sel_inst != -1 ? &trans : nullptr;

	if (sel_inst != -1)
	{
		trans.pos = instances[sel_inst].pos;
		Gizmo::inst->pos2d = instances[sel_inst].pos;
	}

	Gizmo::inst->enabled = (sel_inst != -1);
}
#endif