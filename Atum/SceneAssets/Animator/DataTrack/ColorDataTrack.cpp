
#include "ColorDataTrack.h"
#include "ColorDataTrackWidgets.h"

ColorDataTrack::ColorDataTrack(TrackPlayer* own, const char* nm, Color* set_value, ChangeKeyFunc changeKey_Func) : TemplDataTrack<Color>(own, nm, changeKey_Func)
{
	value = set_value;
}

ColorDataTrack::~ColorDataTrack()
{
	keys.clear();
	values.clear();
}

void ColorDataTrack::Load(JSONReader* stream, int num)
{
	keys.resize(num);
	values.resize(num);

	for (int i=0; i < num; i++)
	{
		if (!stream->EnterBlock("ColorData")) break;

		stream->Read("value", values[i]);
		stream->Read("time", keys[i].time);

		keys[i].blend = DataTrack::BlendLinear;
		stream->Read("blend", (int&)keys[i].blend);
		
		stream->LeaveBlock();
	}
}

void ColorDataTrack::Save(JSONWriter* stream)
{
	stream->StartArray("ColorData");

	for (int i=0; i < keys.size(); i++)
	{
		stream->StartBlock(nullptr);

		stream->Write("value", values[i]);
		stream->Write("time", keys[i].time);
		stream->Write("blend", (int&)keys[i].blend);
		
		stream->FinishBlock();
	}

	stream->FinishArray();
}

void ColorDataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		*value = values[index];
	}
	else
	{
		value->Lerp(values[cur_index-1], values[cur_index], blend);
	}
}

#ifdef EDITOR

void ColorDataTrack::SetDefaultKey(int index)
{
	values[index] = COLOR_WHITE;
}

void ColorDataTrack::InitControls(EUIWidget* parent)
{
	wigets = new ColorDataTrackWidgets();
	wigets->InitControls(parent);
}

void ColorDataTrack::StartEditKey(int key)
{
	if (key != -1)
	{
		wigets->blend = &keys[key].blend;
		wigets->StartEditKey(&values[key]);
	}
}

void ColorDataTrack::StopEditKey()
{
	wigets->StopEditKey();
}
#endif