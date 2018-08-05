
#include "SpriteInst.h"
#include "Services/Render/Render.h"
#include "SceneObjects/RenderLevels.h"

CLASSREG(SceneObject, SpriteInst, "Sprite")

META_DATA_DESC(SpriteInst)
BASE_SCENE_OBJ_NAME_PROP(SpriteInst)
BASE_SCENE_OBJ_STATE_PROP(SpriteInst)
FLOAT_PROP(SpriteInst, trans.pos.x, 100.0f, "Geometry", "PosX")
FLOAT_PROP(SpriteInst, trans.pos.y, 100.0f, "Geometry", "PosY")
FLOAT_PROP(SpriteInst, axis_scale, 1.0f, "Geometry", "axis_scale")
BOOL_PROP(SpriteInst, allow_instances, false, "Geometry", "allow_instances")
FLOAT_PROP(SpriteInst, trans.depth, 0.5f, "Geometry", "Depth")
BOOL_PROP(SpriteInst, frame_state.horz_flipped, false, "Node", "horz_flipped")
META_DATA_DESC_END()

void SpriteInst::BindClassToScript()
{
	BIND_TYPE_TO_SCRIPT(SpriteInst)
	scripts.engine->RegisterObjectMethod(scriptClassName.c_str(), "void AddInstance(float x, float y)", WRAP_MFN(SpriteInst, AddInstance), asCALL_GENERIC);
}

void SpriteInst::Init()
{
	RenderTasks(false)->AddTask(RenderLevels::Sprites, this, (Object::Delegate)&SpriteInst::Draw);
}

void SpriteInst::ApplyProperties()
{
	frame_state.cur_time = -1.0f;
	asset = (SpriteAsset*)owner->FindByUID(asset_uid, 0, true);

	if (StringUtils::IsEqual(asset->GetName(), "DiamondItem"))
	{
		owner->AddToGroup(this, "DiamondItem");
	}

	if (StringUtils::IsEqual(asset->GetName(), "CherryItem"))
	{
		owner->AddToGroup(this, "CherryItem");
	}

	if (StringUtils::IsEqual(asset->GetName(), "EagleFly"))
	{
		owner->AddToGroup(this, "EagleFly");
	}
}

void SpriteInst::Load(JSONReader& loader)
{
	SceneObject::Load(loader);
	loader.Read("asset_uid", asset_uid);
}

void SpriteInst::Save(JSONWriter& saver)
{
	SceneObject::Save(saver);
	saver.Write("asset_uid", asset_uid);
}

void SpriteInst::Play()
{
	float scale = 1.0f / 50.0f;

	if (StringUtils::IsEqual(asset->GetName(), "Ground1_N"))
	{
		b2BodyDef bodyDef;
		bodyDef.position.Set(trans.pos.x * scale, trans.pos.y * scale);

		body = PScene2D()->CreateBody(&bodyDef);

		b2PolygonShape box;
		box.SetAsBox(trans.size.x * 0.5f * scale, trans.size.y * 0.5f * scale);

		body->CreateFixture(&box, 0.0f);
	}
}

void SpriteInst::Stop()
{
	if (body)
	{
		PScene2D()->DestroyBody(body);
		body = nullptr;
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

	trans.BuildMatrices();

	if (!allow_instances || !owner->Playing())
	{
		Sprite::Draw(&trans, COLOR_WHITE, &asset->sprite, &frame_state, true, false);
	}
	else
	if (allow_instances || owner->Playing())
	{
		for (int i = 0; i<instances.size(); i++)
		{
			Instance& inst = instances[i];

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

			trans.pos.x = inst.x;
			trans.pos.y = inst.y;
			
			trans.BuildMatrices();

			Sprite::Draw(&trans, COLOR_WHITE, &asset->sprite, &inst.frame_state, true, false);
		}
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

	if (body)
	{
		b2Vec2 position = body->GetPosition();
		trans.pos.x = position.x * 50.0f;
		trans.pos.y = position.y * 50.0f;

		trans.rotation = body->GetAngle();
	}
}

void SpriteInst::AddInstance(float x, float y)
{
	Instance inst;
	inst.x = x;
	inst.y = y;
	inst.frame_state.looped = false;
	instances.push_back(inst);
}

#ifdef EDITOR
bool SpriteInst::CheckSelection(Vector2 ms)
{
	float scale = render.GetDevice()->GetHeight() / 1024.0f;
	Vector2 pos = trans.pos + trans.offset * trans.size * -1.0f;
	pos *= scale;
	pos.x -= Sprite::ed_cam_pos.x;
	pos.y -= Sprite::ed_cam_pos.y;

	if (pos.x < ms.x && ms.x < pos.x + trans.size.x * scale &&
		pos.y < ms.y && ms.y < pos.y + trans.size.y * scale)
	{
		return true;
	}

	return false;
}

void SpriteInst::Copy(SceneObject* src)
{
	asset_uid = ((SpriteInst*)src)->asset_uid;
	SceneObject::Copy(src);
}

void SpriteInst::SetEditMode(bool ed)
{
	Gizmo::inst->trans2D = ed ? &trans : nullptr;
	if (ed)
	{
		Gizmo::inst->pos2d = trans.pos;
	}
	Gizmo::inst->enabled = ed;
}
#endif