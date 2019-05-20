
#include "Services/Core/Core.h"
#include "SoundStream.h"

#include <algorithm>

bool SoundStream::Load(const char* file_name)
{
	if (!decoded_buffer.Load(file_name, false))
	{
		return false;
	}

	CreateSoundBuffer(2 * CHUNK_SIZE);

	events.resize(2);

	events[0] = CreateEvent(nullptr, false, false, nullptr);
	events[1] = CreateEvent(nullptr, false, false, nullptr);

	DSBPOSITIONNOTIFY notificationMarker[2];

	notificationMarker[0].dwOffset = 0;
	notificationMarker[0].hEventNotify = events[0];

	notificationMarker[1].dwOffset = CHUNK_SIZE;
	notificationMarker[1].hEventNotify = events[1];

	HRESULT result = play_marker->SetNotificationPositions(2, notificationMarker);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundStream::Fill()
{
	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize = 0;

	sound_buffer->Lock(CHUNK_SIZE * buffer_index, CHUNK_SIZE, (void**)&bufferPtr, (DWORD*)&bufferSize, nullptr, 0, 0);

	if (decoded_buffer.Decode(bufferPtr, CHUNK_SIZE, play_type == PlayType::Looped) != CHUNK_SIZE)
	{
		playing = false;
	}

	sound_buffer->Unlock((void*)bufferPtr, bufferSize, nullptr, 0);

	buffer_index = 1 - buffer_index;
}

bool SoundStream::Play(PlayType type)
{
	buffer_index = 0;
	Fill();

	return SoundBase::Play(type, true);
}

void SoundStream::Update()
{
	switch (WaitForMultipleObjects((DWORD)events.size(), &(events[0]), FALSE, 0))
	{
		case WAIT_OBJECT_0:
		case WAIT_OBJECT_0 + 1:
		{
			if (!playing)
			{
				Stop();
			}
			else
			{
				Fill();
			}
		}
		break;
	}

	SoundBase::Update();
}