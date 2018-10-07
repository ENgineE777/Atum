
#include "Transform2DDataTrack.h"
#include "Transform2DDataTrackWidgets.h"

Transform2DDataTrack::Transform2DDataTrack(TrackPlayer* own, const char* nm, Transform2D* set_value, ChangeKeyFunc changeKey_Func) : TemplDataTrack<Transform2D>(own, nm, changeKey_Func)
{
	value = set_value;
#ifdef EDITOR
	def_value.pos = 50.0f;
	def_value.size = 100.0f;
	def_value.rotation = 0.0f;
	blendRage = DataTrack::AllowedBlendAll;
#endif
}

Transform2DDataTrack::~Transform2DDataTrack()
{
	keys.clear();
	values.clear();
}

void Transform2DDataTrack::Load(JSONReader& stream, int num)
{
	keys.resize(num);
	values.resize(num);

	for (int i=0; i<num; i++)
	{
		if (!stream.EnterBlock("Transform2DData")) break;

		stream.Read("pos", values[i].pos);
		stream.Read("size", values[i].size);
		stream.Read("rotation", values[i].rotation);
		stream.Read("time", keys[i].time);

		keys[i].blend = DataTrack::BlendLinear;
		stream.Read("blend", (int&)keys[i].blend);

		stream.LeaveBlock();
	}
}

void Transform2DDataTrack::Save(JSONWriter& stream)
{
	stream.StartArray("Transform2DData");

	for (int i=0;i<keys.size();i++)
	{
		stream.StartBlock(nullptr);

		stream.Write("pos", values[i].pos);
		stream.Write("size", values[i].size);
		stream.Write("rotation", values[i].rotation);
		stream.Write("time", keys[i].time);
		stream.Write("blend", (int&)keys[i].blend);
		
		stream.FinishBlock();
	}
}

void Transform2DDataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		value->pos = values[index].pos;
		value->size = values[index].size;
		value->rotation = values[index].rotation;
	}
	else
	{
		value->pos.Lerp(values[cur_index - 1].pos, values[cur_index].pos, blend);
		value->size.Lerp(values[cur_index - 1].size, values[cur_index].size, blend);
		value->rotation = values[cur_index - 1].rotation + (values[cur_index].rotation - values[cur_index - 1].rotation) * blend;
	}
}

#ifdef EDITOR

void Transform2DDataTrack::InitControls(EUIWidget* parent)
{
	wigets = new Transform2DDataTrackWidgets();
	wigets->InitControls(parent);
}

void Transform2DDataTrack::StartEditKey(int key)
{
	edited_key = key;
	
	if (key!=-1)
	{
		wigets->blend = &keys[key].blend;
		wigets->StartEditKey(&values[key]);
		Gizmo::inst->SetTrans2D(&values[key]);
	}
}

void Transform2DDataTrack::StopEditKey()
{
	edited_key = -1;
	Gizmo::inst->Disable();

	if (wigets)
	{
		wigets->StopEditKey();
	}
}

#endif
