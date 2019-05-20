
#include "Services/Core/Core.h"
#include "SoundInstance.h"

bool SoundInstance::Load(const char* file_name)
{
	if (!decoded_buffer.Load(file_name, true))
	{
		return false;
	}

	CreateSoundBuffer(decoded_buffer.decoded_data_size);

	events.resize(1);

	events[0] = CreateEvent(nullptr, false, false, nullptr);

	DSBPOSITIONNOTIFY notificationMarker[1];

	notificationMarker[0].dwOffset = decoded_buffer.decoded_data_size - 1;
	notificationMarker[0].hEventNotify = events[0];

	HRESULT result = play_marker->SetNotificationPositions(1, notificationMarker);
	if (FAILED(result))
	{
		return false;
	}

	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize = 0;

	result = sound_buffer->Lock(0, decoded_buffer.decoded_data_size, (void**)&bufferPtr, (DWORD*)&bufferSize, nullptr, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	memcpy(bufferPtr, decoded_buffer.decoded_data, decoded_buffer.decoded_data_size);

	result = sound_buffer->Unlock((void*)bufferPtr, bufferSize, nullptr, 0);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundInstance::Play(PlayType type)
{
	return SoundBase::Play(type, type == PlayType::Looped);
}

void SoundInstance::Update()
{
	switch (WaitForMultipleObjects((DWORD)events.size(), &(events[0]), FALSE, 0))
	{
	case WAIT_OBJECT_0:
	{
		if (!playing)
		{
			Stop();
		}
	}
	break;
	}

	SoundBase::Update();
}