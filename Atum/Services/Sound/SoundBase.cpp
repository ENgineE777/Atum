
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

#ifdef PLATFORM_ANDROID
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = decoded_buffer.num_channels;

	switch (decoded_buffer.sample_rate)
	{
		case 8000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_8;
			break;
		}
		case 11025:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_11_025;
			break;
		}
		case 12000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_12;
			break;
		}
		case 16000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_16;
			break;
		}
		case 22050:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_22_05;
			break;
		}
		case 24000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_24;
			break;
		}
		case 32000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_32;
			break;
		}
		case 44100:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
			break;
		}
		case 48000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_48;
			break;
		}
		case 64000:
		{
			format_pcm.samplesPerSec = SL_SAMPLINGRATE_64;
			break;
		}
	}

	switch (decoded_buffer.bits_per_sample)
	{
		case 8:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
			break;
		}
		case 16:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
			break;
		}
		case 20:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_20;
			break;
		}
		case 24:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_24;
			break;
		}
		case 28:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_28;
			break;
		}
		case 32:
		{
			format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_32;
			break;
		}
	}

	format_pcm.containerSize = format_pcm.bitsPerSample;
	format_pcm.channelMask = (format_pcm.numChannels == 2) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_BACK_CENTER;
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	SLDataSource audioSrc = { &loc_bufq, &format_pcm };

	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, core.sounds.outputMixObject };
	SLDataSink audioSnk = { &loc_outmix, nullptr };

	const SLInterfaceID ids[2] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
	const SLboolean req[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
	SLresult result = (*core.sounds.engineEngine)->CreateAudioPlayer(core.sounds.engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 2, ids, req);

	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
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

#ifdef PLATFORM_ANDROID
	(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
#endif

	return true;
}

void SoundBase::Pause(bool pause)
{
#ifdef PLATFORM_PC
	if (sound_buffer)
	{
		if (pause)
		{
			sound_buffer->Stop();
		}
		else
		{
			Play(play_type);
		}
	}
#endif

#ifdef PLATFORM_ANDROID
	(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, pause ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
#endif
}

void SoundBase::Stop()
{
#ifdef PLATFORM_PC
	if (sound_buffer)
	{
		sound_buffer->Stop();
		sound_buffer->SetCurrentPosition(0);
	}
#endif

#ifdef PLATFORM_ANDROID
	(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
#endif

	decoded_buffer.RestartDecode();

	playing = false;
}

void SoundBase::SetVolume(float set_volume)
{
	volume = set_volume;
}

float SoundBase::GetVolume()
{
	return volume;
}

void SoundBase::Update()
{
	if (playing)
	{
#ifdef PLATFORM_PC
		float vol = DSBVOLUME_MAX * volume * core.sounds.master_volume;
		sound_buffer->SetVolume((long)vol);
#endif

#ifdef PLATFORM_ANDROID
		float level = (float)(SL_MILLIBEL_MAX - SL_MILLIBEL_MAX) * volume * core.sounds.master_volume;
		(*bqPlayerVolume)->SetVolumeLevel(bqPlayerVolume, (int)level);
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

#ifdef PLATFORM_ANDROID
	if (bqPlayerObject != nullptr)
	{
		(*bqPlayerObject)->Destroy(bqPlayerObject);
	}
#endif

	decoded_buffer.Clear();

	auto interator = std::lower_bound(core.sounds.sounds.begin(), core.sounds.sounds.end(), this);
	core.sounds.sounds.erase(interator);

	delete this;
}