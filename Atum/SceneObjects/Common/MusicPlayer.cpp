#include "MusicPlayer.h"

CLASSREG(SceneObject, MusicPlayer, "MusicPlayer")

META_DATA_DESC(MusicPlayer)
BASE_SCENE_OBJ_PROP(MusicPlayer)
FILENAME_PROP(MusicPlayer, sound_name, "", "Property", "SoundName")
BOOL_PROP(MusicPlayer, faded, false, "Property", "Faded", " Is music faded")
META_DATA_DESC_END()

void MusicPlayer::Init()
{
	Tasks(false)->AddTask(100, this, (Object::Delegate)&MusicPlayer::Work);
}

void MusicPlayer::SetState(State state)
{
	if (stream && stream->IsPlaying() && !faded)
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

	brief = "Fade In music\n"
	"\n"
	"\\param[in] time time of fade in\n";

	core.scripts.RegisterObjectMethod(script_class_name, "void FadeIn(float time)", WRAP_MFN(MusicPlayer, FadeIn), brief);

	brief = "Fade Out music\n"
	"\n"
	"\\param[in] time time of fade out\n";

	core.scripts.RegisterObjectMethod(script_class_name, "void FadeOut(float time)", WRAP_MFN(MusicPlayer, FadeOut), brief);
}

bool MusicPlayer::Play()
{
	stream = core.sounds.CreateStream(GetScene(), sound_name.c_str());

	if (state == State::Active && !faded)
	{
		stream->Play(SoundBase::Looped);
	}

	return true;
}

void MusicPlayer::FadeIn(float time)
{
	faded = false;

	fade_mode = 1;
	cur_fade_time = 0.0f;
	fade_time = time;

	if (!stream->IsPlaying())
	{
		stream->SetVolume(0.0f);
		stream->Play(SoundBase::Looped);
	}
	else
	{
		stream->Pause(false);
	}
}

void MusicPlayer::FadeOut(float time)
{
	if (faded)
	{
		return;
	}

	fade_mode = -1;
	cur_fade_time = 0.0f;
	fade_time = time;
}

void MusicPlayer::Work(float dt)
{
	if (!stream)
	{
		return;
	}

	if (fade_mode != 0)
	{
		cur_fade_time += dt;

		if (cur_fade_time > fade_time)
		{
			cur_fade_time = fade_time;

			if (fade_mode == -1)
			{
				faded = true;
				stream->Pause(true);
			}
			else
			{
				stream->SetVolume(1.0f);
			}

			fade_mode = 0;
		}
		else
		{
			float k = cur_fade_time / fade_time;

			if (fade_mode == -1)
			{
				k = 1.0f - k;
			}

			stream->SetVolume(k);
		}
	}
}

void MusicPlayer::Release()
{
	RELEASE(stream);

	SceneObject::Release();
}