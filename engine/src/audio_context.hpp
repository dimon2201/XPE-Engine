#pragma once

#include <audio_core.hpp>
#include <sndfile_context.hpp>

namespace xpe
{
    namespace audio
    {
        namespace context
        {
            const u32 k_Channels = 1;

            const u32 k_NumCaptureBuffers = 4;
            const u32 k_CaptureFrequency = 22050;
            const u32 k_CaptureBufferSize = k_CaptureFrequency * k_Channels * sizeof(signed char) / 8; // Divided by 8 only to reduce the buffer size and also to reduce the write time

            inline void* Context = nullptr;
            inline void* PlaybackDevice = nullptr;
            inline void* RecordDevice = nullptr;

            ENGINE_API void InitAudio();
            ENGINE_API void FreeAudio();

            ENGINE_API eAudioError GetError();

            ENGINE_API void GenSources(u32 count, u32* sourceID);
            ENGINE_API void GenBuffers(u32 count, u32* bufferID);

            ENGINE_API void DeleteSources(u32 count, u32* sourceID);
            ENGINE_API void DeleteBuffers(u32 count, u32* bufferID);

            ENGINE_API void PlaySource(u32 sourceID);
            ENGINE_API void StopSource(u32 sourceID);
            ENGINE_API void BindBuffers(u32 sourceID, u32 bufferID);
            ENGINE_API void UnbindBuffers(u32 sourceID);

            ENGINE_API void UploadFileToBuffer(const sAudioFile* file, u32 bufferID);
            ENGINE_API void UpdateBuffer(const sAudioFile* file, u32 sourceID, u32 bufferID, s16* data, s64 frames, bool processed);

            ENGINE_API void StartRecord();
            ENGINE_API void StopRecord();
            ENGINE_API void GetCaptureSamples(s32 size, s32& samples);
            ENGINE_API void UploadSamplesToBuffer(signed char* data, s32 samples);
            ENGINE_API void UpdateBuffers(u32 source, u32* buffer, signed char* data, s32 samples, s32 samplerate);
            ENGINE_API void AddBuffer(u32 source, u32* buffer, signed char* data, s32 samples, s32 samplerate, u32 num);

            ENGINE_API void SetListenerPosition(glm::vec3 position);
            ENGINE_API void SetListenerVelocity(glm::vec3 velocity);
            ENGINE_API void SetListenerOrientation(glm::vec3 front, glm::vec3 up);
            ENGINE_API void SetListenerGain(f32 volume);
            ENGINE_API void SetListenerDopplerFactor(f32 factor);
            ENGINE_API void SetListenerSpeedOfSound(f32 speed);

            ENGINE_API void SetLooping(u32 sourceID, bool Looping);

            ENGINE_API void SetPitch(u32 sourceID, f32 pitch);
            ENGINE_API void SetGain(u32 sourceID, f32 volume);

            ENGINE_API void SetPosition(u32 sourceID, glm::vec3 position);
            ENGINE_API void SetVelocity(u32 sourceID, glm::vec3 velocity);

            ENGINE_API void SetRefDistance(u32 sourceID, f32 distance);
            ENGINE_API void SetMaxDistance(u32 sourceID, f32 distance);

            ENGINE_API void SetRollOffFactor(u32 sourceID, f32 rollOffFactor);

            ENGINE_API void SetConeInnerAngle(u32 sourceID, f32 angle);
            ENGINE_API void SetConeOuterAngle(u32 sourceID, f32 angle);

            ENGINE_API void GetState(u32 sourceID, eAudioState& state);
            ENGINE_API void GetProcessed(u32 sourceID, s32* processed);
            ENGINE_API void GetQueued(u32 sourceID, s32* queued);

            ENGINE_API int GetFormat(const sAudioFile& file, u32 channels);
            ENGINE_API u32 GetBufferSize(u32 channels, u32 BufferSamples);
        }
    }
}