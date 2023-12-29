#ifdef OPENAL

#include <audio/core/context.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <alext.h>

namespace xpe {

    namespace audio {

        namespace context {

            static const std::unordered_map<ALint, eAudioState> s_AudioStates =
            {
                { AL_INITIAL, eAudioState::INITIAL },
                { AL_PLAYING, eAudioState::PLAYING },
                { AL_PAUSED,  eAudioState::PAUSED },
                { AL_STOPPED, eAudioState::STOPPED }
            };

            static const std::unordered_map<ALenum, eAudioError> s_AudioErrors =
            {
                { AL_NO_ERROR, eAudioError::NONE },
                { AL_INVALID_NAME, eAudioError::INVALID_NAME },
                { AL_INVALID_ENUM, eAudioError::INVALID_ENUM },
                { AL_INVALID_VALUE, eAudioError::INVALID_VALUE },
                { AL_INVALID_OPERATION, eAudioError::INVALID_OPERATION },
                { AL_OUT_OF_MEMORY, eAudioError::OUT_OF_MEMORY },
            };

            void InitAudio()
            {
                LogInfo("Audio initialize");

                PlaybackDevice = alcOpenDevice(nullptr);
                if (!PlaybackDevice) {
                    LogError("Failed to get playback device");
                }

                Context = alcCreateContext((ALCdevice*) PlaybackDevice, nullptr);
                if (!Context) {
                    LogError("Failed to set audio context");
                }

                if (!alcMakeContextCurrent((ALCcontext*) Context)) {
                    LogError("Failed to make audio context current");
                }

                RecordDevice = alcCaptureOpenDevice(nullptr, k_CaptureFrequency, AL_FORMAT_MONO16, k_CaptureBufferSize);
                if (!RecordDevice) {
                    LogError("Failed to get record device");
                }

                if (PlaybackDevice && Context && RecordDevice) {
                    LogInfo("Audio initialized!");
                }
            }

            void FreeAudio()
            {
                LogInfo("Audio clearing...");

                alcMakeContextCurrent(nullptr);
                alcDestroyContext((ALCcontext*) Context);

                alcCloseDevice((ALCdevice*) PlaybackDevice);

                alcCaptureStop((ALCdevice*) RecordDevice);
                alcCaptureCloseDevice((ALCdevice*) RecordDevice);

                LogInfo("Audio cleared!");
            }

            eAudioError GetError()
            {
                return s_AudioErrors.at(alGetError());
            }

            void GenSources(u32 count, u32* sourceID)
            {
                alGenSources(count, sourceID);
            }

            void GenBuffers(u32 count, u32* bufferID)
            {
                alGenBuffers(count, bufferID);
            }

            void DeleteSources(u32 count, u32* sourceID)
            {
                alDeleteSources(count, sourceID);
            }

            void DeleteBuffers(u32 count, u32* bufferID)
            {
                alDeleteBuffers(count, bufferID);
            }

            void PlaySource(u32 sourceID)
            {
                alSourcePlay(sourceID);

                if (alGetError() != AL_NO_ERROR) {
                    LogError("Error starting playback source {}", sourceID);
                }
            }

            void StopSource(u32 sourceID)
            {
                alSourceStop(sourceID);
            }

            void bindBuffers(u32 sourceID, u32 bufferID)
            {
                alSourcei(sourceID, AL_BUFFER, bufferID);
            }

            void UnbindBuffers(u32 sourceID)
            {
                alSourcei(sourceID, AL_BUFFER, 0);
            }

            void UploadFileToBuffer(const sAudioFile* file, u32 bufferID)
            {
                s64 chunk, size;
                vector<short> data;
                data.reserve(GetBufferSize(file->Info.channels, file->Info.frames));

                chunk = ReadChunk(file->File, data.data(), file->Info.frames);
                SetCurrentFrame(file->File, 0);

                if (chunk > 1) {
                    size = chunk * file->Info.channels * 2; // 2 == sizeof(short);
                    alBufferData(bufferID, file->Info.format, data.data(), size, file->Info.samplerate);
                }

                else {
                    LogError("Can't update file data into buffer {}", bufferID);
                }
            }

            void UpdateBuffer(const sAudioFile* file, u32 sourceID, u32 bufferID, s16* data, s64 frames, bool processed)
            {
                s32 queued;
                s64 chunk, size;

                GetQueued(sourceID, &queued);

                if (processed && queued) {
                    alSourceUnqueueBuffers(sourceID, 1, &bufferID);
                }

                chunk = ReadChunk(file->File, data, frames);

                if (chunk) {
                    size = chunk * file->Info.channels * 2; // 2 == sizeof(short);
                    alBufferData(bufferID, file->Info.format, data, size, file->Info.samplerate);
                    alSourceQueueBuffers(sourceID, 1, &bufferID);
                }

                else {
                    LogError("Cant update file chunk into buffer {}", bufferID);
                }
            }

            void StartRecord()
            {
                alcCaptureStart((ALCdevice*)RecordDevice);
            }

            void StopRecord()
            {
                alcCaptureStop((ALCdevice*) RecordDevice);
            }

            void GetCaptureSamples(s32 size, s32& samples)
            {
                alcGetIntegerv((ALCdevice*) RecordDevice, ALC_CAPTURE_SAMPLES, size, &samples);
            }

            void UploadSamplesToBuffer(signed char* data, s32 samples)
            {
                alcCaptureSamples((ALCdevice*) RecordDevice, data, samples);
            }

            void UpdateBuffers(u32 source, u32* buffer, signed char* data, s32 samples, s32 samplerate)
            {
                alSourceUnqueueBuffers(source, 1, buffer);
                alBufferData(*buffer, AL_FORMAT_MONO16, data, samples * 2, samplerate);
                alSourceQueueBuffers(source, 1, buffer);
            }

            void AddBuffer(u32 source, u32* buffer, signed char* data, s32 samples, s32 samplerate, u32 num)
            {
                for (int i = 0; i < num; ++i) {
                    alBufferData(*buffer, AL_FORMAT_MONO16, data, samples, samplerate);
                }

                alSourceQueueBuffers(source, 1, buffer);
            }

            void SetListenerPosition(glm::vec3 position)
            {
                alListener3f(AL_POSITION, position.x, position.y, position.z);
            }

            void SetListenerVelocity(glm::vec3 velocity)
            {
                alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
            }

            void SetListenerOrientation(glm::vec3 front, glm::vec3 up)
            {
                float orientation[6] = { front.x, front.y, front.z, up.x, up.y, up.z };
                alListenerfv(AL_ORIENTATION, orientation);
            }

            void SetListenerGain(f32 volume)
            {
                alListenerf(AL_GAIN, volume);
            }

            void SetListenerDopplerFactor(f32 factor)
            {
                alListenerf(AL_DOPPLER_FACTOR, factor);
            }

            void SetListenerSpeedOfSound(f32 speed)
            {
                alListenerf(AL_SPEED_OF_SOUND, speed);
            }

            void SetLooping(u32 sourceID, bool Looping)
            {
                alSourcei(sourceID, AL_LOOPING, Looping);
            }

            void SetPitch(u32 sourceID, f32 pitch)
            {
                alSourcef(sourceID, AL_PITCH, pitch);
            }

            void SetGain(u32 sourceID, f32 volume)
            {
                alSourcef(sourceID, AL_GAIN, volume);
            }

            void SetPosition(u32 sourceID, glm::vec3 position)
            {
                alSource3f(sourceID, AL_POSITION, position.x, position.y, position.z);
            }

            void SetVelocity(u32 sourceID, glm::vec3 velocity)
            {
                alSource3f(sourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
            }

            void SetRefDistance(u32 sourceID, f32 distance)
            {
                alSourcef(sourceID, AL_REFERENCE_DISTANCE, distance);
            }

            void SetMaxDistance(u32 sourceID, f32 distance)
            {
                alSourcef(sourceID, AL_MAX_DISTANCE, distance);
            }

            void SetRollOffFactor(u32 sourceID, f32 rollOffFactor)
            {
                alSourcef(sourceID, AL_ROLLOFF_FACTOR, rollOffFactor);
            }

            void SetConeInnerAngle(u32 sourceID, f32 angle)
            {
                alSourcef(sourceID, AL_CONE_INNER_ANGLE, angle);
            }

            void SetConeOuterAngle(u32 sourceID, f32 angle)
            {
                alSourcef(sourceID, AL_CONE_OUTER_ANGLE, angle);
            }

            void GetState(u32 sourceID, eAudioState& state)
            {
                ALint alState;
                alGetSourcei(sourceID, AL_SOURCE_STATE, &alState);
                state = s_AudioStates.at(alState);
            }

            void GetProcessed(u32 sourceID, s32* processed)
            {
                alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, processed);
            }

            void GetQueued(u32 sourceID, s32* queued)
            {
                alGetSourcei(sourceID, AL_BUFFERS_QUEUED, queued);
            }

            int GetFormat(const sAudioFile& file, u32 channels)
            {
                if (channels == 1) {
                    return AL_FORMAT_MONO16;
                }

                else if (channels == 2) {
                    return AL_FORMAT_STEREO16;
                }

                else if (isAmbisonic(file)) {

                    if (channels == 3) {
                        return AL_FORMAT_BFORMAT2D_16;
                    }

                    else if (channels == 4) {
                        return AL_FORMAT_BFORMAT3D_16;
                    }

                }

                else {
                    LogError("Unsupported channels={} for file", channels);
                }

                sf_close(file.File);

                return AL_NONE;
            }

            u32 GetBufferSize(u32 channels, u32 BufferSamples)
            {
                return BufferSamples * channels * sizeof(short);
            }

        }

    }

}

#endif // OPENAL