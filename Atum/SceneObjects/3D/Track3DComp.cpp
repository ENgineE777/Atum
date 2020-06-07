
#include "Track3DComp.h"
#include "MeshInstance.h"

#ifdef EDITOR
#include "Editor/Editor.h"
#endif

COMPREG(Track3DComp, "Track3D")
COMPINCL(MeshInstance)
COMPREG_END(Track3DComp)

META_DATA_DESC(Track3DComp::Point)
	FLOAT_PROP(Track3DComp::Point, trans._41, 0.0f, "Prop", "x", "X coordinate of a position")
	FLOAT_PROP(Track3DComp::Point, trans._42, 0.0f, "Prop", "y", "Y coordinate of a position")
	FLOAT_PROP(Track3DComp::Point, trans._43, 0.0f, "Prop", "z", "Z coordinate of a position")
	FLOAT_PROP(Track3DComp::Point, wait_time, 0.0f, "Prop", "wait_time", "wait time")
META_DATA_DESC_END()

META_DATA_DESC(Track3DComp::Track)
	ENUM_PROP(Track3DComp::Track, tp, 0, "Prop", "data_type", "Type of track")
		ENUM_ELEM("OneWay", 0)
		ENUM_ELEM("ForwardBack", 1)
		ENUM_ELEM("Looped", 2)
	ENUM_END
	FLOAT_PROP(Track3DComp::Track, speed, 50.0f, "Prop", "speed", "Speed of moving along track")
	FLOAT_PROP(Track3DComp::Track, rotation_speed, 1.0f, "Prop", "angle_speed", "Speed of rotation")
	ARRAY_PROP_INST_CALLGIZMO(Track3DComp::Track, points, Point, "Prop", "track", Track3DComp, sel_point, SetGizmo)
META_DATA_DESC_END()


META_DATA_DESC(Track3DComp)
	ARRAY_PROP_INST(Track3DComp, tracks, Track, "Tracks", "tracks", Track3DComp, sel_track)
META_DATA_DESC_END()

void Track3DComp::Track::Activate(bool set_active)
{
	active = set_active;
}

void Track3DComp::Track::ResetScript()
{
	Reset(true);
}

void Track3DComp::Track::Reset(bool from_start)
{
	if (points.size() > 1)
	{
		cur_point = from_start ? 1 : (int)points.size() - 1;
		cur_dist = 0.0f;
		auto dir = points[cur_point].trans.Pos() - points[cur_point - 1].trans.Pos();
		point_dist = dir.Length();
		dir = from_start ? 1.0f : -1.0f;
		lerp_time = 0.0f;
	}
}

void Track3DComp::Track::SetGizmo()
{
	if (sel_point)
	{
		Gizmo::inst->SetTrans3D(&points[*sel_point].trans);
	}
}

void Track3DComp::Track::PredictPosition(float dt, Vector3& pos)
{
	float saved_dir = dir;
	int saved_cur_point = cur_point;
	float saved_cur_dist = cur_dist;
	float saved_point_dist = point_dist;
	float saved_wait_time = wait_time;
	float saved_lerp_time = lerp_time;

	Update(dt);

	pos = cur_pos;

	dir = saved_dir;
	cur_point = saved_cur_point;
	cur_dist = saved_cur_dist;
	point_dist = saved_point_dist;
	wait_time = saved_wait_time;
	lerp_time = saved_lerp_time;
}

void Track3DComp::Track::Update(float dt)
{
	if (!active || points.size() < 2)
	{
		return;
	}

	if (wait_time < -0.5f)
	{
		cur_dist += speed * dt;

		while (cur_dist > point_dist)
		{
			cur_dist -= point_dist;

			if (dir > 0.0f)
			{
				cur_point++;

				if (cur_point >= points.size())
				{
					if (tp == OneWay)
					{
						Reset(true);
					}
					else
						if (tp == ForwardBack)
						{
							Reset(false);
						}
						else
						{
							if (cur_point == points.size())
							{
								point_dist = (points[cur_point - 1].trans.Pos() - points[0].trans.Pos()).Length();
							}
							else
							{
								Reset(true);
							}
						}
				}
				else
				{
					point_dist = (points[cur_point].trans.Pos() - points[cur_point - 1].trans.Pos()).Length();
				}
			}
			else
			{
				cur_point--;

				if (cur_point < 1)
				{
					Reset(true);
				}
				else
				{
					point_dist = (points[cur_point].trans.Pos() - points[cur_point - 1].trans.Pos()).Length();
				}
			}

			lerp_time = 1.0f;

			int index = 0;

			if ((tp == Looped && cur_point == points.size()) || dir > 0.0f)
			{
				index = cur_point - 1;
			}
			else
			{
				index = cur_point;
			}

			if (points[index].wait_time > 0.01f)
			{
				wait_time = points[index].wait_time;
				break;
			}
		}
	}
	else
	{
		wait_time -= dt;

		if (wait_time < 0.0f)
		{
			wait_time = -1.0f;
		}
	}

	int p1 = 0;
	int p2 = 0;

	if (tp == Looped && cur_point == points.size())
	{
		p1 = cur_point - 1;
		p2 = 0;
	}
	else
	{
		if (dir > 0.0f)
		{
			p1 = cur_point - 1;
			p2 = cur_point;
		}
		else
		{
			p1 = cur_point;
			p2 = cur_point - 1;
		}
	}

	auto& point1 = points[p1];
	auto& point2 = points[p2];

	cur_dir = point2.trans.Pos() - point1.trans.Pos();
	cur_pos = cur_dir * cur_dist / point_dist + point1.trans.Pos();
}

void Track3DComp::BindClassToScript()
{
	const char* brief = "Representation of 3D track component\n"
		"\n"
		"This class is adding 3D track to each instance of SpriteInst.\n"
		"\n"
		"This class ::Track2DComp is a representation on C++ side.\n";

	core.scripts.RegisterObjectType(script_class_name, sizeof(Track3DComp::Track), "gr_script_scene_object_components", brief);
	core.scripts.RegisterObjectMethod(script_class_name, "void Activate(bool set)", WRAP_MFN(Track3DComp::Track, Activate), "Make active or inactive track (instance isn't moving along a inactive track)");
	core.scripts.RegisterObjectMethod(script_class_name, "void Reset()", WRAP_MFN(Track3DComp::Track, ResetScript), "Reset track");
	core.scripts.RegisterObjectMethod(script_class_name, "void PredictPosition(float dt, Vector3&out)", WRAP_MFN(Track3DComp::Track, PredictPosition), "Predict Position");
}

void Track3DComp::InjectIntoScript(asIScriptObject* object, int index, const char* prefix)
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

void Track3DComp::SyncInstances()
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	if (mesh_inst->instances.size() != tracks.size())
	{
		tracks.resize(mesh_inst->instances.size());
	}
}

void Track3DComp::Play()
{
	object->Tasks(false)->AddTask(-100, this, (Object::Delegate)&Track3DComp::UpdateInstances);
}

void Track3DComp::UpdateInstances(float dt)
{
	for (int index = 0; index < tracks.size(); index++)
	{
		UpdateTrack(index, dt);
	}
}

void Track3DComp::UpdateTrack(int index, float dt)
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	if (mesh_inst->GetState() != SceneObject::State::Active)
	{
		return;
	}

	auto& inst = mesh_inst->instances[index];

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

	track.Update(dt);

#ifdef EDITOR
	if (IsEditMode() || object->IsEditMode())
	{
		core.render.DebugSphere(track.cur_pos, COLOR_MAGNETA, 0.5f);

		//ed_angle = angle;
		//core.render.DebugLine2D(Sprite::MoveToCamera(pos), COLOR_WHITE, Sprite::MoveToCamera(pos + Vector2(cosf(angle), sinf(angle)) * 50.0f), COLOR_WHITE);

		if (sel_track == index)
		{
			if (sel_point == 0)
			{
				mesh_inst->instances[sel_track].mesh->transform.Pos() = track.points[0].trans.Pos();
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
		track.lerp_time -= dt * track.rotation_speed;

		if (track.lerp_time > 0.0f)
		{
			track.cur_dir.SLerp(track.cur_dir, track.prev_dir, track.lerp_time);
		}
		else
		{
			track.prev_dir = track.cur_dir;
		}

		Matrix mat;
		mat.BuildView(track.cur_pos, track.cur_pos + track.cur_dir, Vector3(0.0f, 1.0f, 0.0f));
		mat.Inverse();

		inst.mesh->transform = mat;
	}
}

#ifdef EDITOR
void Track3DComp::Init()
{
	object->Tasks(false)->AddTask(0, this, (Object::Delegate)&Track3DComp::EditorDraw);
}

void Track3DComp::ApplyProperties()
{
	for (auto& track : tracks)
	{
		track.Reset(true);
	}
}

void Track3DComp::EditorDraw(float dt)
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	if (!IsEditMode() && object->IsEditMode())
	{
		sel_track = ((MeshInstance*)object)->sel_inst;
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
				mesh_inst->instances[sel_track].mesh->transform = track.points[0].trans;
			}

			sel_point = -1;
			SetGizmo();
		}

		bool add_center = core.controls.DebugKeyPressed("KEY_P");
		bool add_copy = core.controls.DebugKeyPressed("KEY_O");

		if (add_center || (add_copy && sel_point != -1))
		{
			Point point;

			if (add_copy)
			{
				point.trans = track.points[sel_point].trans;
				point.trans.Pos().x += 1.0f;
			}
			else
			{
				point.trans.Pos() += editor.freecamera.pos + Vector3(cosf(editor.freecamera.angles.x), sinf(editor.freecamera.angles.y), sinf(editor.freecamera.angles.x)) * 5.0f;
			}

			track.points.push_back(point);

			sel_point = (int)track.points.size() - 1;

			SetGizmo();

			if (track.points.size() )
			{
				mesh_inst->instances[sel_track].mesh->transform = track.points[0].trans;
			}
		}
	}

	for (int i = 0; i < track.points.size(); i++)
	{
		Vector3 p1 = track.points[i].trans.Pos();

		core.render.DebugSphere(p1, COLOR_GREEN, 0.5f);

		if (i != 0)
		{
			Vector3 p2 = track.points[i - 1].trans.Pos();
			core.render.DebugLine(p1, COLOR_GREEN, p2, COLOR_GREEN);
		}
		else
		if (track.tp == Looped)
		{
			Vector3 p2 = track.points[track.points.size() - 1].trans.Pos();
			core.render.DebugLine(p1, COLOR_GREEN, p2, COLOR_CYAN);
		}
	}

	UpdateTrack(sel_track, dt);
}

void Track3DComp::ResizeInst(int count)
{
	tracks.resize(count);
}

void Track3DComp::SyncPosTrackWithInstPos(int index)
{
	MeshInstance* mesh_inst = (MeshInstance*)object;
	Track& track = tracks[index];

	Vector3 delta = mesh_inst->instances[index].mesh->transform.Pos() - track.points[0].trans.Pos();

	for (auto& point : track.points)
	{
		point.trans.Pos() += delta;
	}
}

void Track3DComp::InstAdded(int create_from)
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

void Track3DComp::InstDeleted(int index)
{
	tracks.erase(tracks.begin() + index);
}

void Track3DComp::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

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

		if (Math::IntersectSphereRay(point.trans.Pos(), 0.5f, start, dir))
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

void Track3DComp::SetEditMode(bool ed)
{
	SceneObjectComp::SetEditMode(ed);

	if (ed)
	{
		sel_track = ((MeshInstance*)object)->sel_inst;
		SetGizmo();
	}
	else
	{
		sel_track = -1;
		sel_point = -1;
	}
}

void Track3DComp::SetGizmo()
{
	MeshInstance* mesh_inst = (MeshInstance*)object;

	if (sel_track == -1)
	{
		return;
	}

	Track& track = tracks[sel_track];

	if (sel_point != -1)
	{
		Gizmo::inst->SetTrans3D(&track.points[sel_point].trans);
	}
	else
	{
		Gizmo::inst->Disable();
	}
}
#endif