#pragma once

class SoundBase;

class IAudioQueueHolder
{
public:

    virtual void Fill(SoundBase* sound) = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SetVolume(float volume) = 0;
    virtual void Release() = 0;
};

extern void CreateAudioQueueHolder(SoundBase* sound, bool is_stream);
