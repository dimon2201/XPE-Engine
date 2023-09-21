#include <audio/core/openal_context.h>
#include <audio/core/config.h>

namespace xpe {

	namespace audio {

		namespace context {

			void InitAudio()
			{
				LogInfo("Audio initialize");

				PlaybackDevice = alcOpenDevice(nullptr); // get payback device
				if (!PlaybackDevice) {
					LogInfo("failed to get playback device");
				}

				OpenALContext = alcCreateContext(PlaybackDevice, nullptr);  // create context
				if (!OpenALContext) {
					LogInfo("Failed to set audio context");
				}

				if (!alcMakeContextCurrent(OpenALContext)) { // make context current
					LogInfo("failed to make audio context current");
				}

				RecordDevice = alcCaptureOpenDevice(nullptr, SAMPLE_RATE, AL_FORMAT_MONO16, SAMPLE_RATE/2); // get record device
				if (!RecordDevice) {
					LogInfo("failed to get record device");
				}

				if (PlaybackDevice && OpenALContext && RecordDevice) {
					LogInfo("Audio initialized");
				}
			}

			void FreeAudio()
			{
				LogInfo("Audio clearing");

				alcMakeContextCurrent(nullptr);
				alcDestroyContext(OpenALContext);

				alcCloseDevice(PlaybackDevice);

				alcCaptureStop(RecordDevice);
				alcCaptureCloseDevice(RecordDevice);

				LogInfo("Audio cleared");
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

			void PlaySource(u32 sourceID, s32& State)
			{
				alSourcePlay(sourceID);

				if (alGetError() != AL_NO_ERROR) {
					LogInfo("Error starting playback");
				}
			}

			void StopAudio(u32 sourceID)
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

			void UploadFileToBuffer(AudioFile& file, u32 bufferID)
			{
				s64 chunk, size;
				vector<short> data;
				data.reserve(GetBufferSize(file.Info.channels, file.Info.frames));

				chunk = ReadChunk(file.File, data.data(), file.Info.frames);
				SetCurrentFrame(file.File, 0);

				if (chunk > 1) {
					size = chunk * file.Info.channels * 2; // 2 == sizeof(short);
					alBufferData(bufferID, file.Info.format, data.data(), size, file.Info.samplerate);
				}
				else {
					LogInfo("Cant read file!");
				}
			}

			void UpdateBuffer(const AudioFile& file, u32 sourceID, u32 bufferID, short* data, s64 frames, s32 processed)
			{
				s64 chunk, size;

				if (processed) {
					alSourceUnqueueBuffers(sourceID, 1, &bufferID);
				}

				chunk = ReadChunk(file.File, data, frames);

				if (chunk) {
					size = chunk * file.Info.channels * 2; // 2 == sizeof(short);
					alBufferData(bufferID, file.Info.format, data, size, file.Info.samplerate);
					alSourceQueueBuffers(sourceID, 1, &bufferID);
				}
				else {
					LogInfo("Cant read file!");
				}
			}

			void StartRecord(u32 sourceID, u32* buffers, s32 state, short* data, u32 numBuffers) 
			{
				RecordDevice = alcCaptureOpenDevice(nullptr, SAMPLE_RATE, AL_FORMAT_MONO16, SAMPLE_RATE / 2);

				for (int i = 0; i < numBuffers; i++) {
					alBufferData(buffers[i], AL_FORMAT_MONO16, data, DATA_SIZE, SAMPLE_RATE);
				}

				alSourceQueueBuffers(sourceID, numBuffers, buffers);

				PlaySource(sourceID, state);

				alcCaptureStart(RecordDevice);
			}

			void StopRecord()
			{
				alcCaptureStop(RecordDevice);
			}

			void GetCaptureSamples(s32 size, s32& samples)
			{
				alcGetIntegerv(RecordDevice, ALC_CAPTURE_SAMPLES, size, &samples);
			}

			void UploadSamplesToBuffer(short* data, s32 samples)
			{
				alcCaptureSamples(RecordDevice, data, samples);
			}

			void UpdateBuffers(u32 source, u32* buffer, short* data, s32 samples, s32 samplerate) {
				alSourceUnqueueBuffers(source, 1, buffer);
				alBufferData(*buffer, AL_FORMAT_MONO16, data, samples * 2, samplerate);
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

			void SetListenerOrientation(glm::vec3 look, glm::vec3 up)
			{
				float orientation[6] = { look.x, look.y, look.z, up.x, up.y, up.z };
				alListenerfv(AL_ORIENTATION, orientation);
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


			void GetState(u32 sourceID, s32* state)
			{
				alGetSourcei(sourceID, AL_SOURCE_STATE, state);
			}

			void GetProcessed(u32 sourceID, s32* processed)
			{
				alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, processed);
			}

			u32 GetFormat(const AudioFile& file, u32 channels)
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

				LogInfo("Unsupported channel count from file");
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