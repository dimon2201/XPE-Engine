#pragma once

#include <audio/core/core.h>
#include <audio/core/sndfile_context.h>

#include <core/types.hpp>

namespace xpe {

    namespace audio {

        namespace context {

            using namespace xpe::core;

            const u32 k_SampleRate = 22050;
            const u32 k_Channels = 1;
            const f32 k_Duration = 0.02;
            const u32 k_DataSize = static_cast<unsigned int>(k_Duration * k_SampleRate * k_Channels * 2);

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
            ENGINE_API void StopAudio(u32 sourceID);
            ENGINE_API void bindBuffers(u32 sourceID, u32 bufferID);
            ENGINE_API void UnbindBuffers(u32 sourceID);

            ENGINE_API void UploadFileToBuffer(sAudioFile& file, u32 bufferID);
            ENGINE_API void UpdateBuffer(const sAudioFile& file, u32 sourceID, u32 bufferID, short* data, s64 frames, s32 processed);

            ENGINE_API void StartRecord(u32 sourceID, u32* buffers, s32 state, short* data, u32 numBuffers);
            ENGINE_API void StopRecord();
            ENGINE_API void GetCaptureSamples(s32 size, s32& samples);
            ENGINE_API void UploadSamplesToBuffer(short* data, s32 samples);
            ENGINE_API void UpdateBuffers(u32 source, u32* buffer, short* data, s32 samples, s32 samplerate);

            ENGINE_API void SetListenerPosition(glm::vec3 position);
            ENGINE_API void SetListenerVelocity(glm::vec3 velocity);
            ENGINE_API void SetListenerOrientation(glm::vec3 look, glm::vec3 up);

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

            ENGINE_API int GetFormat(const sAudioFile& file, u32 channels);
            ENGINE_API u32 GetBufferSize(u32 channels, u32 BufferSamples);
        }
    }
}