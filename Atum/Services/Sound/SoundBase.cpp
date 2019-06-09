
#include "Services/Core/Core.h"
#include "SoundBase.h"

#include <algorithm>

void SoundBase::CreateSoundBuffer(long size)
{
#ifdef PLATFORM_PC
	WAVEFORMATEX waveFormat;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = decoded_buffer.sample_rate;
	waveFormat.wBitsPerSample = decoded_buffer.bits_per_sample;
	waveFormat.nChannels = decoded_buffer.num_channels;
	waveFormat.nBlockAlign = decoded_buffer.block_align;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
	bufferDesc.dwBufferBytes = size;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer* tempBuffer;
	HRESULT result = core.sounds.direct_sound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);

	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&sound_buffer);

	tempBuffer->Release();

	result = sound_buffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&play_marker);
#endif
}

bool SoundBase::Play(PlayType type, bool buffer_looped)
{
#ifdef PLATFORM_PC
	if (!sound_buffer)
	{
		return false;
	}
#endif

	play_type = type;
	playing = true;

#ifdef PLATFORM_PC
	HRESULT result = sound_buffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	result = sound_buffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	result = sound_buffer->Play(0, 0, buffer_looped ? DSBPLAY_LOOPING : 0);
	if (FAILED(result))
	{
		return false;
	}
#endif

	return true;
}

void SoundBase::SetVolume(float set_volume)
{
	volume = set_volume;
}

float SoundBase::GetVolume()
{
	return volume;
}

void SoundBase::Stop()
{
#ifdef PLATFORM_PC
	if (sound_buffer)
	{
		sound_buffer->Stop();
	}
#endif

	playing = false;
}

void SoundBase::Update()
{
	if (playing)
	{
#ifdef PLATFORM_PC
		float vol = DSBVOLUME_MAX * volume * core.sounds.master_volume;

		sound_buffer->SetVolume((long)vol);
#endif
	}
}

void SoundBase::Release()
{
	Stop();

#ifdef PLATFORM_PC
	for (auto event : events)
	{
		CloseHandle(event);
	}

	RELEASE(play_marker)

	RELEASE(sound_buffer)
#endif

	decoded_buffer.Clear();

	auto interator = std::lower_bound(core.sounds.sounds.begin(), core.sounds.sounds.end(), this);
	core.sounds.sounds.erase(interator);

	delete this;
}