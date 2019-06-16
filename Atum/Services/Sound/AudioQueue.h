#pragma once

class SoundBase;

void AudioQueue_Add(SoundBase* sound, bool is_stream);
void AudioQueue_Delete(SoundBase* sound);
void AudioQueue_Fill(SoundBase* sound);
void AudioQueue_Play(SoundBase* sound);
void AudioQueue_Pause(SoundBase* sound);
void AudioQueue_Stop(SoundBase* sound);
void AudioQueue_SetVolume(SoundBase* sound, float volume);
