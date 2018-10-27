
#include "Track2DComp.h"
#include "SpriteInst.h"

COMPREG(Track2DComp, "Track2D")
COMPINCL(SpriteInst)
COMPREG_END(Track2DComp)

META_DATA_DESC(Track2DComp::Point)
FLOAT_PROP(Track2DComp::Point, pos.x, 0.0f, "Prop", "x")
FLOAT_PROP(Track2DComp::Point, pos.y, 0.0f, "Prop", "y")
META_DATA_DESC_END()

META_DATA_DESC(Track2DComp::Track)
ENUM_PROP(Track2DComp::Track, tp, 0, "Prop", "data_type")
	ENUM_ELEM("OneWay", 0)
	ENUM_ELEM("ForwardBack", 1)
	ENUM_ELEM("Looped", 2)
ENUM_END
FLOAT_PROP(Track2DComp::Track, speed, 0.5f, "Prop", "speed")
ARRAY_PROP(Track2DComp::Track, points, Point, "Prop", "track")
META_DATA_DESC_END()


META_DATA_DESC(Track2DComp)
ENUM_PROP(Track2DComp, flip_mode, 0, "Prop", "flip_mode")
	ENUM_ELEM("None", 0)
	ENUM_ELEM("Normal", 1)
	ENUM_ELEM("Reversed", 2)
ENUM_END
ARRAY_PROP(Track2DComp, tracks, Track, "Tracks", "tracks")
META_DATA_DESC_END()

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

#ifdef EDITOR
	if (!IsEditMode() && !object->IsEditMode())
#endif
	{
		if (!sprite_inst->instances[index].IsVisible())
		{
			return;
		}
	}

	Track& track = tracks[index];

	if (!track.active || track.points.size() < 2 || dt < 0.001f)
	{
		return;
	}

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
					track.cur_point = 0;
					track.point_dist = 0.0f;
				}
				else
				if (track.tp == ForwardBack)
				{
					track.cur_point = (int)track.points.size() - 1;
					track.point_dist = 0.0f;
					track.dir = -1.0f;
				}
				else
				{
					if (track.cur_point == track.points.size())
					{
						track.point_dist = (track.points[track.cur_point - 1].pos - track.points[0].pos).Length();
					}
					else
					{
						track.cur_point = 0;
						track.point_dist = 0.0f;
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

			if (track.cur_point < 0)
			{
				track.cur_point = 0;
				track.point_dist = 0.0f;
				track.dir = 1.0f;
			}
			else
			{
				track.point_dist = (track.points[track.cur_point].pos - track.points[track.cur_point + 1].pos).Length();
			}
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
		p1 = track.dir > 0.0f ? track.cur_point - 1 : track.cur_point + 1;
		p2 = track.cur_point;
	}

	Vector2 dir = track.points[p2].pos - track.points[p1].pos;
	Vector2 pos = dir * track.cur_dist / track.point_dist + track.points[p1].pos;

#ifdef EDITOR
	if (IsEditMode() || object->IsEditMode())
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;
		render.DebugSprite(nullptr, pos * scale - Sprite::ed_cam_pos - 10.0f + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f, 20.0f, COLOR_BLUE);
	}
	else
#endif
	{
		if (sprite_inst->array)
		{
			asIScriptObject** objects = (asIScriptObject**)sprite_inst->array->GetBuffer();

			*((float*)objects[index]->GetAddressOfProperty(sprite_inst->mapping[0])) = pos.x;
			*((float*)objects[index]->GetAddressOfProperty(sprite_inst->mapping[1])) = pos.y;

			if (flip_mode != FlipMode::None)
			{
				int flip = dir.x > 0.0f ? 0 : 1;

				if (flip_mode == FlipMode::Reversed)
				{
					flip = 1 - flip;
				}

				*((int*)objects[index]->GetAddressOfProperty(sprite_inst->mapping[2])) = flip;
			}
		}
		else
		{
			sprite_inst->instances[index].SetPos(pos);
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
	if (!IsEditMode() && !object->IsEditMode())
	{
		return;
	}

	for (auto& track : tracks)
	{
		track.cur_point = 0;
		track.point_dist = 0.0f;
		track.dir = 1.0f;
	}
}

void Track2DComp::EditorDraw(float dt)
{
	if (!IsEditMode() && !object->IsEditMode())
	{
		return;
	}

	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->sel_inst == -1)
	{
		return;
	}

	Track& track = tracks[sprite_inst->sel_inst];

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	if (IsEditMode())
	{
		if (sel_point != -1)
		{
			track.points[sel_point].pos = trans.pos;
		}

		if (controls.DebugKeyPressed("KEY_I") && sel_point != -1)
		{
			track.points.erase(sel_point + track.points.begin());
			sel_point = -1;
			SetGizmo();
		}

		bool add_center = controls.DebugKeyPressed("KEY_O");
		bool add_after = controls.DebugKeyPressed("KEY_P");

		if (add_center || add_after)
		{
			Point point;

			if (sel_point != -1 && add_after)
			{
				point.pos = track.points[sel_point].pos + 20.0f;
			}
			else
			{
				float scale = 1024.0f / render.GetDevice()->GetHeight();
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
		Vector2 p1 = track.points[i].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
		render.DebugSprite(nullptr, p1 - 15.0f, 30.0f, COLOR_GREEN);

		if (i != 0)
		{
			Vector2 p2 = track.points[i - 1].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
			render.DebugLine2D(p1, COLOR_GREEN, p2, COLOR_GREEN);
		}
		else
		if (track.tp == Looped)
		{
			Vector2 p2 = track.points[track.points.size() - 1].pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;
			render.DebugLine2D(p1, COLOR_GREEN, p2, COLOR_CYAN);
		}
	}

	UpdateTrack(sprite_inst->sel_inst, dt);
}

void Track2DComp::ResizeInst(int count)
{
	tracks.resize(count);
}

void Track2DComp::InstAdded()
{
	tracks.push_back(Track());
}

void Track2DComp::InstDeleted(int index)
{
	tracks.erase(tracks.begin() + index);
}

void Track2DComp::CheckSelection(Vector2 ms)
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->sel_inst == -1)
	{
		return;
	}

	Track& track = tracks[sprite_inst->sel_inst];

	float scale = render.GetDevice()->GetHeight() / 1024.0f;

	sel_point = -1;
	for (int i = 0; i < track.points.size(); i++)
	{
		Point& point = track.points[i];

		Vector2 pos = point.pos * scale - Sprite::ed_cam_pos + Vector2((float)render.GetDevice()->GetWidth(), (float)render.GetDevice()->GetHeight()) * 0.5f;

		if (pos.x - 15.0f< ms.x && ms.x < pos.x + 15.0f &&
			pos.y - 15.0f< ms.y && ms.y < pos.y + 15.0f)
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
		SetGizmo();
	}
}

void Track2DComp::SetGizmo()
{
	SpriteInst* sprite_inst = (SpriteInst*)object;

	if (sprite_inst->sel_inst == -1)
	{
		return;
	}

	Track& track = tracks[sprite_inst->sel_inst];

	if (sel_point != -1)
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;
		trans.size = 60.0f / scale;
		trans.pos = track.points[sel_point].pos;
	}

	Gizmo::inst->SetTrans2D(sel_point != -1 ? &trans : nullptr, Gizmo::trans_2d_move);
}
#endif