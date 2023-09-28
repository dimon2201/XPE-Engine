#pragma once

#include <audio/core/core.h>
#include <audio/core/sndfile_context.h>

#include <core/types.hpp>

namespace xpe {

    namespace audio {

        namespace context {

            using namespace xpe::core;

            //record values
            const u32 SAMPLE_RATE = 22050;
            const u32 CHANNELS = 1;
            const f32 DURATION = 0.02;

            const u32 DATA_SIZE = static_cast<unsigned int>(DURATION * SAMPLE_RATE * CHANNELS * 2);

            inline void* Context = nullptr;
            inline void* PlaybackDevice = nullptr;
            inline void* RecordDevice = nullptr;

            void InitAudio();
            void FreeAudio();

            eAudioError GetError();

            void GenSources(u32 count, u32* sourceID);
            void GenBuffers(u32 count, u32* bufferID);

            void DeleteSources(u32 count, u32* sourceID);
            void DeleteBuffers(u32 count, u32* bufferID);

            void PlaySource(u32 sourceID);
            void StopAudio(u32 sourceID);
            void bindBuffers(u32 sourceID, u32 bufferID);
            void UnbindBuffers(u32 sourceID);

            void UploadFileToBuffer(AudioFile& file, u32 bufferID);
            void UpdateBuffer(const AudioFile& file, u32 sourceID, u32 bufferID, short* data, s64 frames, s32 processed);

            void StartRecord(u32 sourceID, u32* buffers, s32 state, short* data, u32 numBuffers);
            void StopRecord();
            void GetCaptureSamples(s32 size, s32& samples);
            void UploadSamplesToBuffer(short* data, s32 samples);
            void UpdateBuffers(u32 source, u32* buffer, short* data, s32 samples, s32 samplerate);

            void SetListenerPosition(glm::vec3 position);
            void SetListenerVelocity(glm::vec3 velocity);
            void SetListenerOrientation(glm::vec3 look, glm::vec3 up);

            void SetLooping(u32 sourceID, bool Looping);

            void SetPitch(u32 sourceID, f32 pitch);
            void SetGain(u32 sourceID, f32 volume);

            void SetPosition(u32 sourceID, glm::vec3 position);
            void SetVelocity(u32 sourceID, glm::vec3 velocity);

            void SetRefDistance(u32 sourceID, f32 distance);
            void SetMaxDistance(u32 sourceID, f32 distance);

            void SetRollOffFactor(u32 sourceID, f32 rollOffFactor);

            void SetConeInnerAngle(u32 sourceID, f32 angle);
            void SetConeOuterAngle(u32 sourceID, f32 angle);

            void GetState(u32 sourceID, eAudioState& state);
            void GetProcessed(u32 sourceID, s32* processed);

            int ENGINE_API GetFormat(const AudioFile& file, u32 channels);
            u32 ENGINE_API GetBufferSize(u32 channels, u32 BufferSamples);
        }
    }
}