#include "Services/Core/Core.h"
#include <AudioToolbox/AudioToolbox.h>

struct AudioQueueHolder : public IAudioQueueHolder
{
public:
    
    constexpr static int num_buffer = 3;
    constexpr static int buffer_size = 4096;
    bool is_stream;
    uint64_t current_pos = 0;
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[num_buffer];
    
    static void AudioQueueCallback(void* data, AudioQueueRef queue, AudioQueueBufferRef buffer);

    void CreateQueue(SoundBase* sound, bool set_is_stream);
    void Fill(SoundBase* sound) override;
    void FillDecoded(SoundBase* sound, uint8_t* buffer);
    void Play() override;
    void Pause() override;
    void Stop() override;
    void SetVolume(float volume)  override;
    void Release()  override;
};

void AudioQueueHolder::AudioQueueCallback(void* data, AudioQueueRef queue, AudioQueueBufferRef buffer)
{
    SoundBase* sound = (SoundBase*)data;
    
    if (!sound->playing)
    {
        sound->Stop();
    }
    else
    {
        AudioQueueHolder* holder = (AudioQueueHolder*)sound->audio_queue;
        
        if (holder->is_stream)
        {
            if (sound->decoded_buffer.Decode((uint8_t*)buffer->mAudioData, AudioQueueHolder::buffer_size, sound->play_type == SoundBase::PlayType::Looped) != AudioQueueHolder::buffer_size)
            {
                sound->playing = false;
            }
        }
        else
        {
            holder->FillDecoded(sound, (uint8_t*)buffer->mAudioData);
            
            if (sound->play_type != SoundBase::PlayType::Looped &&
                holder->current_pos == sound->decoded_buffer.decoded_data_size)
            {
                sound->playing = false;
            }
        }
        
        AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
    }
}

void AudioQueueHolder::CreateQueue(SoundBase* sound, bool set_is_stream)
{
    sound->audio_queue = this;
    
    is_stream = set_is_stream;
    
    AudioStreamBasicDescription format;
    
    format.mSampleRate       = sound->decoded_buffer.sample_rate;
    format.mFormatID         = kAudioFormatLinearPCM;
    format.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    format.mBitsPerChannel   = sound->decoded_buffer.bits_per_sample;
    format.mChannelsPerFrame = sound->decoded_buffer.num_channels;
    format.mBytesPerFrame    = sound->decoded_buffer.block_align;
    format.mFramesPerPacket  = 1;
    format.mBytesPerPacket   = format.mBytesPerFrame * format.mFramesPerPacket;
    format.mReserved         = 0;
    
    AudioQueueNewOutput(&format, AudioQueueCallback, sound, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &queue);

    for (int i = 0; i < AudioQueueHolder::num_buffer; i++)
    {
        AudioQueueAllocateBuffer(queue, AudioQueueHolder::buffer_size, &buffers[i]);
    }
}

void AudioQueueHolder::FillDecoded(SoundBase* sound, uint8_t* buffer)
{
    uint64_t size = AudioQueueHolder::buffer_size;
    
    if (current_pos + size > sound->decoded_buffer.decoded_data_size)
    {
        size = sound->decoded_buffer.decoded_data_size - current_pos;
    }
    
    if (size > 0)
    {
        memcpy(buffer, sound->decoded_buffer.decoded_data + current_pos, size);
        current_pos += size;
    }
    
    if (size != AudioQueueHolder::buffer_size)
    {
        buffer += size;
        
        size = AudioQueueHolder::buffer_size - size;
        
        if (sound->play_type == SoundBase::PlayType::Looped)
        {
            current_pos = 0;
            
            memcpy(buffer, sound->decoded_buffer.decoded_data + current_pos, size);
            
            current_pos += size;
        }
        else
        {
            memset(buffer, 0, size);
        }
    }
}

void AudioQueueHolder::Fill(SoundBase* sound)
{
    for (int i = 0; i < AudioQueueHolder::num_buffer; i++)
    {
        buffers[i]->mAudioDataByteSize = AudioQueueHolder::buffer_size;
        
        if (is_stream)
        {
            sound->decoded_buffer.Decode((uint8_t*)buffers[i]->mAudioData, AudioQueueHolder::buffer_size, sound->play_type == SoundBase::PlayType::Looped);
        }
        else
        {
            FillDecoded(sound, (uint8_t*)buffers[i]->mAudioData);
        }
        
        AudioQueueEnqueueBuffer(queue, buffers[i], 0, nullptr);
    }
}

void AudioQueueHolder::Play()
{
    AudioQueueStart(queue, nullptr);
}

void AudioQueueHolder::Pause()
{
    AudioQueuePause(queue);
}

void AudioQueueHolder::Stop()
{
    AudioQueueStop(queue, false);
}

void AudioQueueHolder::SetVolume(float volume)
{
    AudioQueueSetParameter(queue, kAudioQueueParam_Volume, volume);
}

void AudioQueueHolder::Release()
{
    for (int i = 0; i < AudioQueueHolder::num_buffer; i++)
    {
        AudioQueueFreeBuffer(queue, buffers[i]);
    }
    
    AudioQueueDispose(queue, false);
    
    delete this;
}

void CreateAudioQueueHolder(SoundBase* sound, bool is_stream)
{
    AudioQueueHolder* audio_quene = new AudioQueueHolder();
    audio_quene->CreateQueue(sound, is_stream);
}
