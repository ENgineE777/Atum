
#include "Services/Core/Core.h"
#include "SoundBase.h"

#include <algorithm>

void SoundBase::CreateSoundBuffer(long size)
{
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
}

bool SoundBase::Play(PlayType type, bool buffer_looped)
{
	if (!sound_buffer)
	{
		return false;
	}

	play_type = type;
	playing = true;

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
	if (sound_buffer)
	{
		sound_buffer->Stop();
	}

	playing = false;
}

void SoundBase::Update()
{
	if (playing)
	{
		float vol = DSBVOLUME_MAX * volume * core.sounds.master_volume;
		sound_buffer->SetVolume((long)vol);
	}
}

void SoundBase::Release()
{
	Stop();

	for (auto event : events)
	{
		CloseHandle(event);
	}

	RELEASE(play_marker)

	RELEASE(sound_buffer)

	decoded_buffer.Clear();

	auto& interator = std::lower_bound(core.sounds.sounds.begin(), core.sounds.sounds.end(), this);
	core.sounds.sounds.erase(interator);

	delete this;
}