
#include "TrackPlayer.h"
#include "../Animator.h"

CLASSFACTORYDECL(TrackPlayer)

TrackPlayer::TrackPlayer()
{
	name[0] = 0;
}

TrackPlayer::~TrackPlayer()
{
	Clear();
}

void TrackPlayer::Clear()
{
	for (int i=0; i<tracks.size();i++)
	{
		delete tracks[i];
	}

	tracks.clear();
}

#ifdef EDITOR
void TrackPlayer::InitControls(EUIWidget* parent)
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->InitControls(parent);
	}
}

void TrackPlayer::DataFromGizmo()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->DataFromGizmo();
	}
}

void TrackPlayer::DebugDraw()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->DebugDraw();
	}
}

void TrackPlayer::SetActive(bool value)
{
	active = value;
}
#endif

void TrackPlayer::Load(JSONReader& stream)
{
	stream.Read("Name", name, 64);
	stream.Read("Active", active);

	GetMetaData()->Prepare(this);
	GetMetaData()->Load(stream);

	int count = 0;
	stream.Read("Count", count);

	for (int i=0; i<count;i++)
	{
		if (!stream.EnterBlock("DataTrack")) break;

		int num_keys = 0;
		stream.Read("Count", num_keys);

		char trackName[64];
		stream.Read("Name", trackName, 64);

		tracks[i]->Load(stream, num_keys);

		stream.LeaveBlock();
	}
}

void TrackPlayer::Save(JSONWriter& stream)
{
	stream.Write("Type", type);
	stream.Write("Name", name);
	stream.Write("Active", active);

	GetMetaData()->Prepare(this);
	GetMetaData()->Save(stream);

	int count = (int)tracks.size();
	stream.Write("Count", count);

	stream.StartArray("DataTrack");

	for (int i=0; i<count;i++)
	{
		stream.StartBlock(NULL);

		int num_keys = tracks[i]->GetKeysCount();
		stream.Write("Count", num_keys);

		stream.Write("Name", tracks[i]->GetName());

		tracks[i]->Save(stream);

		stream.FinishBlock();
	}

	stream.FinishArray();
}

void TrackPlayer::Reset()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->Reset();
	}
}

void TrackPlayer::SetTime(float time)
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->SetTime(time);
	}
}

void TrackPlayer::Release()
{
	Clear();

	delete this;
}

DataTrack* TrackPlayer::GetDataTrack( const char* _name )
{
	int size = (int)tracks.size();
	for (int i = 0; i < size; ++i)
	{
		if (StringUtils::IsEqual(tracks[i]->GetName(), _name))
			return tracks[i];
	}

	return NULL;
}