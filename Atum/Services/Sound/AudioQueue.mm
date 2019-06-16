#include "Services/Core/Core.h"
#include <AudioToolbox/AudioToolbox.h>

struct AudioQueueHolder
{
    constexpr static int num_buffer = 3;
    constexpr static int buffer_size = 4096;
    bool is_stream;
    uint64_t current_pos = 0;
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[num_buffer];
    void CreateQueue(SoundBase* sound);
    void Fill(SoundBase* sound);
    void FillDecoded(SoundBase* sound, uint8_t* buffer);
    static void AudioQueueCallback(void* data, AudioQueueRef queue, AudioQueueBufferRef buffer);
};

map<SoundBase*, AudioQueueHolder> audio_queues;

void AudioQueueHolder::AudioQueueCallback(void* data, AudioQueueRef queue, AudioQueueBufferRef buffer)
{
    SoundBase* sound = (SoundBase*)data;
    
    if (!sound->playing)
    {
        sound->Stop();
    }
    else
    {
        AudioQueueHolder& holder = audio_queues[sound];
        
        if (holder.is_stream)
        {
            if (sound->decoded_buffer.Decode((uint8_t*)buffer->mAudioData, AudioQueueHolder::buffer_size, sound->play_type == SoundBase::PlayType::Looped) != AudioQueueHolder::buffer_size)
            {
                sound->playing = false;
            }
        }
        else
        {
            holder.FillDecoded(sound, (uint8_t*)buffer->mAudioData);
            
            if (sound->play_type != SoundBase::PlayType::Looped &&
                holder.current_pos == sound->decoded_buffer.decoded_data_size)
            {
                sound->playing = false;
            }
        }
        
        AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
    }
}

void AudioQueueHolder::CreateQueue(SoundBase* sound)
{
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

void AudioQueue_Add(SoundBase* sound, bool is_stream)
{
    AudioQueueHolder& holder = audio_queues[sound];
    
    holder.is_stream = is_stream;
    holder.CreateQueue(sound);
}

void AudioQueue_Delete(SoundBase* sound)
{
    AudioQueueHolder& holder = audio_queues[sound];
    
    for (int i = 0; i < AudioQueueHolder::num_buffer; i++)
    {
        AudioQueueFreeBuffer(holder.queue, holder.buffers[i]);
    }
    
    AudioQueueDispose(holder.queue, false);

    audio_queues.erase(sound);
}

void AudioQueue_Fill(SoundBase* sound)
{
    AudioQueueHolder& holder = audio_queues[sound];
    holder.Fill(sound);
}

void AudioQueue_Play(SoundBase* sound)
{
    AudioQueueHolder& holder = audio_queues[sound];
    AudioQueueStart(holder.queue, nullptr);
}

void AudioQueue_Pause(SoundBase* sound)
{
    AudioQueueHolder& holder = audio_queues[sound];
    AudioQueuePause(holder.queue);
}

void AudioQueue_Stop(SoundBase* sound)
{
    AudioQueueHolder& holder = audio_queues[sound];
    AudioQueueStop(holder.queue, false);
}

void AudioQueue_SetVolume(SoundBase* sound, float volume)
{
    AudioQueueHolder& holder = audio_queues[sound];
    AudioQueueSetParameter(holder.queue, kAudioQueueParam_Volume, volume);
}
