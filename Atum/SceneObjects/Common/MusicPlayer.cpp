
#include "MusicPlayer.h"

CLASSREG(SceneObject, MusicPlayer, "MusicPlayer")

META_DATA_DESC(MusicPlayer)
BASE_SCENE_OBJ_PROP(MusicPlayer)
FILENAME_PROP(MusicPlayer, sound_name, "", "Property", "SoundName")
BOOL_PROP(MusicPlayer, play_on_start, true, "Property", "PlayOnStart", "Should sound play on start")
META_DATA_DESC_END()

void MusicPlayer::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&MusicPlayer::Work);
}

void MusicPlayer::SetState(State state)
{
	if (stream && stream->IsPlaying())
	{
		stream->Pause(state != State::Active);
	}

	SceneObject::SetState(state);
}

void MusicPlayer::BindClassToScript()
{
	const char* brief = "Representation of a music player\n"
	"\n"
	"This object plays streamed sound.\n";

	BIND_TYPE_TO_SCRIPT(MusicPlayer, brief)
}

bool MusicPlayer::Play()
{
	stream = core.sounds.CreateStream(sound_name.c_str());

	if (play_on_start)
	{
		stream->Play(SoundBase::Looped);
	}

	return true;
}

void MusicPlayer::Work(float dt)
{

}

void MusicPlayer::Release()
{
	RELEASE(stream);

	SceneObject::Release();
}