
#include "Track2DComp.h"
#include "SpriteInst.h"

COMPREG(Track2DComp, "Track2D")
COMPINCL(SpriteInst)
COMPREG_END(Track2DComp)

META_DATA_DESC(Track2DComp::Point)
	FLOAT_PROP(Track2DComp::Point, pos.x, 0.0f, "Prop", "x", "X coordinate of a position")
	FLOAT_PROP(Track2DComp::Point, pos.y, 0.0f, "Prop", "y", "Y coordinate of a position")
	FLOAT_PROP(Track2DComp::Point, angle, -1.0f, "Prop", "angle", "angle")
	FLOAT_PROP(Track2DComp::Point, wait_time, 0.0f, "Prop", "wait_time", "wait time")
META_DATA_DESC_END()

META_DATA_DESC(Track2DComp::Track)
	ENUM_PROP(Track2DComp::Track, tp, 0, "Prop", "data_type", "Type of track")
		ENUM_ELEM("OneWay", 0)
		ENUM_ELEM("ForwardBack", 1)
		ENUM_ELEM("Looped", 2)
	ENUM_END
	FLOAT_PROP(Track2DComp::Track, speed, 50.0f, "Prop", "speed", "Speed of moving along track")
	FLOAT_PROP(Track2DComp::Track, angle_speed, -1.0f, "Prop", "angle_speed", "Angle speed of rotation")
	ARRAY_PROP_INST_CALLGIZMO(Track2DComp::Track, points, Point, "Prop", "track", Track2DComp, sel_point, SetGizmo)
META_DATA_DESC_END()


META_DATA_DESC(Track2DComp)
	ENUM_PROP(Track2DComp, flip_mode, 0, "Prop", "flip_mode", "Type of flip when instance is moving from right to left")
		ENUM_ELEM("None", 0)
		ENUM_ELEM("Normal", 1)
		ENUM_ELEM("Reversed", 2)
	ENUM_END
	ARRAY_PROP_INST(Track2DComp, tracks, Track, "Tracks", "tracks", Track2DComp, sel_track)
META_DATA_DESC_END()

void Track2DComp::Track::Activate(bool set_active)
{
	active = set_active;
}

void Track2DComp::Track::ResetScript()
{
	Reset(true);
}

void Track2DComp::Track::Reset(bool from_start)
{
	if (points.size() > 1)
	{
		cur_point = from_start ? 1 : (int)points.size() - 1;
		cur_dist = 0.0f;
		point_dist = (points[cur_point].pos - points[cur_point - 1].pos).Length();
		dir = from_start ? 1.0f : -1.0f;
	}
}

void Track2DComp::Track::SetGizmo()
{
	if (sel_point)
	{
		trans_size = 60.0f / Sprite::ed_cam_zoom;

		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(&points[(*sel_point)].pos, &trans_size), Gizmo::trans_2d_move);
	}
}

void Track2DComp::BindClassToScript()
{
	const char* brief = "Representation of 2D track component\n"
		"\n"
		"This class is adding 2D track to each instance of SpriteInst.\n"
		"\n"
		"This class ::Track2DComp is a representation on C++ side.\n";

	core.scripts.RegisterObjectType(script_class_name, sizeof(Track2DComp::Track), "gr_script_scene_object_components", brief);
	core.scripts.RegisterObjectMethod(script_class_name, "void Activate(bool set)", WRAP_MFN(Track2DComp::Track, Activate), "Make active or inactive track (instance isn't moving along a inactive track)");
	core.scripts.RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(Track2DComp::Track, ResetScript), "Reset track");

}

void Track2DComp::InjectIntoScript(asIScriptObject* object, int index, const char* prefix)
{
	if (prop_index == -1)
	{
		char str[256];

		if (prefix[0])
		{
			StringUtils::Printf(str, 256, "%s_track", prefix);

			for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual(str, object->GetPropertyName(prop)))
				{
					prop_index = prop;
					break;
				}
			}
		}

		if (prop_index == -1)
		{
			for (int prop = 0; prop < (int)object->GetPropertyCount(); prop++)
			{
				if (StringUtils::IsEqual("track", object->GetPropertyName(prop)))
				{
					prop_index = prop;
					break;
				}
			}
		}
	}

	if (prop_index != -1)
	{
		SyncInstances();

		*(asPWORD*)(object->GetAddressOfProperty(prop_index)) = (asPWORD)&tracks[index];
	}
}

void Track2DComp::SyncInstances()
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->instances.size() != tracks.size())
	{
		tracks.resize(sprite_inst->instances.size());
	}
}

void Track2DComp::Play()
{
	object->Tasks(false)->AddTask(-100, this, (Object::Delegate)&Track2DComp::UpdateInstances);
}

void Track2DComp::UpdateInstances(float dt)
{
	for (int index = 0; index < tracks.size(); index++)
	{
		UpdateTrack(index, dt);
	}
}

void Track2DComp::UpdateTrack(int index, float dt)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->GetState() != SceneObject::State::Active)
	{
		return;
	}

	auto& inst = sprite_inst->instances[index];

#ifdef EDITOR
	if (!IsEditMode() && !object->IsEditMode())
#endif
	{
		if (!inst.IsVisible())
		{
			return;
		}
	}

	Track& track = tracks[index];

	if (!track.active || track.points.size() < 2)
	{
		return;
	}

	if (track.wait_time < -0.5f)
	{
		track.cur_dist += track.speed * dt;

		while (track.cur_dist > track.point_dist)
		{
			track.cur_dist -= track.point_dist;

			if (track.dir > 0.0f)
			{
				track.cur_point++;

				if (track.cur_point >= track.points.size())
				{
					if (track.tp == OneWay)
					{
						track.Reset(true);
					}
					else
					if (track.tp == ForwardBack)
					{
						track.Reset(false);
					}
					else
					{
						if (track.cur_point == track.points.size())
						{
							track.point_dist = (track.points[track.cur_point - 1].pos - track.points[0].pos).Length();
						}
						else
						{
							track.Reset(true);
						}
					}
				}
				else
				{
					track.point_dist = (track.points[track.cur_point].pos - track.points[track.cur_point - 1].pos).Length();
				}
			}
			else
			{
				track.cur_point--;

				if (track.cur_point < 1)
				{
					track.Reset(true);
				}
				else
				{
					track.point_dist = (track.points[track.cur_point].pos - track.points[track.cur_point - 1].pos).Length();
				}
			}

			int index = 0;

			if ((track.tp == Looped && track.cur_point == track.points.size()) || track.dir > 0.0f)
			{
				index = track.cur_point - 1;
			}
			else
			{
				index = track.cur_point;
			}

			if (track.points[index].wait_time > 0.01f)
			{
				track.wait_time = track.points[index].wait_time;
				break;
			}
		}
	}
	else
	{
		track.wait_time -= dt;

		if (track.wait_time < 0.0f)
		{
			track.wait_time = -1.0f;
		}
	}

	int p1 = 0;
	int p2 = 0;

	if (track.tp == Looped && track.cur_point == track.points.size())
	{
		p1 = track.cur_point - 1;
		p2 = 0;
	}
	else
	{
		if (track.dir > 0.0f)
		{
			p1 = track.cur_point - 1;
			p2 = track.cur_point;
		}
		else
		{
			p1 = track.cur_point;
			p2 = track.cur_point - 1;
		}
	}

	auto& point1 = track.points[p1];
	auto& point2 = track.points[p2];

	Vector2 dir = point2.pos - point1.pos;
	Vector2 pos = dir * track.cur_dist / track.point_dist + point1.pos;

	float len = dir.Length();

	float angle = 0.0f;

	if (point1.angle > -0.01f || len > 0.0001f)
	{
		float target_angle = point1.angle > -0.01f ? (point1.angle * Math::Radian) : atan2(dir.y / len, dir.x / len);

#ifdef EDITOR
		if (IsEditMode() || object->IsEditMode())
		{
			angle = ed_angle;
		}
		else
#endif
		{
			angle = inst.GetAngle();
		}

		if (flip_mode != FlipMode::None || track.angle_speed > 0.01f)
		{
			angle = 0;
		}
		else
		{
			angle = Math::AdvanceAngle(angle, target_angle, dt * track.angle_speed * Math::Radian);
		}
	}

#ifdef EDITOR
	if (IsEditMode() || object->IsEditMode())
	{
		core.render.DebugSprite(nullptr, Sprite::MoveToCamera(pos) - 10.0f, 20.0f, COLOR_BLUE);

		ed_angle = angle;
		core.render.DebugLine2D(Sprite::MoveToCamera(pos), COLOR_WHITE, Sprite::MoveToCamera(pos + Vector2(cosf(angle), sinf(angle)) * 50.0f), COLOR_WHITE);

		if (sel_track == index)
		{
			if (sel_point == 0)
			{
				sprite_inst->instances[sel_track].SetPos(track.points[0].pos);
			}
			else
			{
				SyncPosTrackWithInstPos(sel_track);
			}
		}
	}
	else
#endif
	{
		inst.SetPos(pos);
		inst.SetAngle(angle);

		if (flip_mode != FlipMode::None)
		{
			int flip = dir.x > 0.0f ? 0 : 1;

			if (flip_mode == FlipMode::Reversed)
			{
				flip = 1 - flip;
			}

			inst.SetFlipped(flip);
		}
	}
}

#ifdef EDITOR
void Track2DComp::Init()
{
	object->Tasks(false)->AddTask(0, this, (Object::Delegate)&Track2DComp::EditorDraw);
}

void Track2DComp::ApplyProperties()
{
	for (auto& track : tracks)
	{
		track.Reset(true);
	}
}

void Track2DComp::EditorDraw(float dt)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (!IsEditMode() && object->IsEditMode())
	{
		sel_track = ((SpriteInst*)object)->sel_inst;
	}

	if (!IsEditMode() && !object->IsEditMode())
	{
		return;
	}

	if (sel_track == -1)
	{
		return;
	}

	Track& track = tracks[sel_track];
	track.sel_point = IsEditMode() ? &sel_point : nullptr;

	if (IsEditMode())
	{
		if (core.controls.DebugKeyPressed("KEY_I") && sel_point != -1)
		{
			track.points.erase(sel_point + track.points.begin());
			track.Reset(true);

			if (sel_point == 0 && track.points.size() > 0)
			{
				sprite_inst->instances[sel_track].SetPos(track.points[0].pos);
			}

			sel_point = -1;
			SetGizmo();
		}

		bool add_center = core.controls.DebugKeyPressed("KEY_P");
		bool add_copy = core.controls.DebugKeyPressed("KEY_O");

		if (add_center || (add_copy && sel_point != -1))
		{
			Point point;

			if (sel_point != -1 && add_copy)
			{
				point = track.points[sel_point];
				point.pos = track.points[sel_point].pos + 20.0f;
			}
			else
			{
				float scale = 1024.0f / core.render.GetDevice()->GetHeight();
				point.pos.x = Sprite::ed_cam_pos.x * scale;
				point.pos.y = Sprite::ed_cam_pos.y * scale;
			}

			track.points.push_back(point);

			sel_point = (int)track.points.size() - 1;

			SetGizmo();
		}
	}

	for (int i = 0; i < track.points.size(); i++)
	{
		Vector2 p1 = Sprite::MoveToCamera(track.points[i].pos);

		core.render.DebugSprite(nullptr, p1 - 15.0f, 30.0f, COLOR_GREEN);

		if (i != 0)
		{
			Vector2 p2 = Sprite::MoveToCamera(track.points[i - 1].pos);
			core.render.DebugLine2D(p1, COLOR_GREEN, p2, COLOR_GREEN);
		}
		else
		if (track.tp == Looped)
		{
			Vector2 p2 = Sprite::MoveToCamera(track.points[track.points.size() - 1].pos);
			core.render.DebugLine2D(p1, COLOR_GREEN, p2, COLOR_CYAN);
		}
	}

	for (int i = 0; i < track.points.size(); i++)
	{
		Vector2 p1 = Sprite::MoveToCamera(track.points[i].pos);

		if (track.points[i].angle > -0.01f)
		{
			float angle = track.points[i].angle * Math::Radian;
			Vector2 p2 = Sprite::MoveToCamera(track.points[i].pos + Vector2(cosf(angle), sinf(angle)) * 50.0f);
			core.render.DebugLine2D(p1, COLOR_YELLOW, p2, COLOR_YELLOW);
		}
	}


	UpdateTrack(sel_track, dt);
}

void Track2DComp::ResizeInst(int count)
{
	tracks.resize(count);
}

void Track2DComp::SyncPosTrackWithInstPos(int index)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;
	Track& track = tracks[index];

	Vector2 delta = sprite_inst->instances[index].GetPos() - track.points[0].pos;

	for (auto& point : track.points)
	{
		point.pos += delta;
	}
}

void Track2DComp::InstAdded(int create_from)
{
	tracks.push_back(Track());

	if (create_from != -1)
	{
		tracks.back() = tracks[create_from];
		tracks.back().Reset(true);

		SyncPosTrackWithInstPos((int)tracks.size() - 1);

		sel_point = -1;
	}
}

void Track2DComp::InstDeleted(int index)
{
	tracks.erase(tracks.begin() + index);
}

void Track2DComp::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sel_track == -1)
	{
		return;
	}

	Track& track = tracks[sel_track];

	ms = Sprite::MoveFromCamera(ms);
	float sz = Sprite::ScaleToAbs(15.0f);

	sel_point = -1;
	for (int i = 0; i < track.points.size(); i++)
	{
		Point& point = track.points[i];

		Vector2 pos = point.pos;

		if (pos.x - sz < ms.x && ms.x < pos.x + sz &&
			pos.y - sz < ms.y && ms.y < pos.y + sz)
		{
			sel_point = i;

			if (edited)
			{
				SetGizmo();
			}

			return;
		}
	}
}

void Track2DComp::SetEditMode(bool ed)
{
	SceneObjectComp::SetEditMode(ed);

	if (ed)
	{
		sel_track = ((SpriteInst*)object)->sel_inst;
		SetGizmo();
	}
	else
	{
		sel_track = -1;
		sel_point = -1;
	}
}

void Track2DComp::SetGizmo()
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sel_track == -1)
	{
		return;
	}

	Track& track = tracks[sel_track];

	if (sel_point != -1)
	{
		trans.size = 60.0f / Sprite::ed_cam_zoom;

		Gizmo::inst->SetTrans2D(Gizmo::Transform2D(&track.points[sel_point].pos, &trans.size), Gizmo::trans_2d_move);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}
#endif