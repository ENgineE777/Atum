
#include "FloatDataTrack.h"
#include "FloatDataTrackWidgets.h"
#include "../AnimatorWindow.h"

FloatDataTrack::FloatDataTrack(TrackPlayer* own, const char* nm, float* set_value, ChangeKeyFunc changeKey_Func) : TemplDataTrack<float>(own, nm, changeKey_Func)
{
	value = set_value;
	def_val = 0.0f;
#ifdef EDITOR
	DataTrack::AllowedBlend blendRage = DataTrack::AllowedBlendAll;
#endif
}

FloatDataTrack::~FloatDataTrack()
{
	keys.clear();
}

void FloatDataTrack::Load(JSONReader* stream, int num)
{
	keys.resize(num);
	values.resize(num);

	for (int i=0; i < num; i++)
	{
		if (!stream->EnterBlock("FloatData")) break;

		stream->Read("val", values[i]);
		stream->Read("time", keys[i].time);

		keys[i].blend = DataTrack::BlendLinear;
		stream->Read("blend", (int&)keys[i].blend);
		
		stream->LeaveBlock();
	}
}

void FloatDataTrack::Save(JSONWriter* stream)
{
	stream->StartArray("FloatData");

	for (int i=0; i < keys.size(); i++)
	{
		stream->StartBlock(nullptr);

		stream->Write("val", values[i]);
		stream->Write("time", keys[i].time);
		stream->Write("blend", (int&)keys[i].blend);
		
		stream->FinishBlock();
	}

	stream->FinishArray();
}

void FloatDataTrack::CalcValue(int index, float blend)
{
	if (blend < -0.01f)
	{
		*value = values[index];
	}
	else
	{
		*value = values[cur_index-1] + (values[cur_index] - values[cur_index-1]) * blend;
	}
}

#ifdef EDITOR

void FloatDataTrack::SetDefaultKey(int index)
{
	values[index] = def_val;
}

void FloatDataTrack::InitControls(EUIWidget* parent)
{
	wigets = new FloatDataTrackWidgets();
	wigets->InitControls(parent);
}

void FloatDataTrack::StartEditKey(int key)
{
	if (key != -1)
	{
		wigets->blend = &keys[key].blend;
		wigets->StartEditKey(&values[key], name);
	}
}

void FloatDataTrack::StopEditKey()
{
	wigets->StopEditKey();
}
#endif